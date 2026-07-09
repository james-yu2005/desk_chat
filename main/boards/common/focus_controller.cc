#include "focus_controller.h"

#include "application.h"
#include "board.h"
#include "focus_presence.h"
#include "assets/lang_config.h"

#include <esp_log.h>
#include <cJSON.h>

static const char* TAG = "FocusController";

FocusController& FocusController::GetInstance() {
    static FocusController instance;
    return instance;
}

void FocusController::Initialize() {
    auto& mcp_server = McpServer::GetInstance();

    mcp_server.AddTool(
        "self.focus.start",
        "Start a focus timer. Uses the camera to detect if you leave your desk and speaks up "
        "to stop you. The screen stays unchanged. Default duration is 25 minutes.",
        PropertyList({
            Property("minutes", kPropertyTypeInteger, 25, 1, 180),
            Property("seconds", kPropertyTypeInteger, 0, 0, 59)
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            int minutes = properties["minutes"].value<int>();
            int seconds = properties["seconds"].value<int>();
            if (!Start(minutes, seconds)) {
                throw std::runtime_error("Failed to start focus session");
            }
            return GetStatusJson();
        });

    mcp_server.AddTool(
        "self.focus.stop",
        "Stop the current focus timer.",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            (void)properties;
            Stop();
            return GetStatusJson();
        });

    mcp_server.AddTool(
        "self.focus.status",
        "Get the current focus session status including remaining time and desk presence.",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            (void)properties;
            return GetStatusJson();
        });

    ESP_LOGI(TAG, "FocusController initialized");
}

bool FocusController::Start(int minutes, int seconds) {
    if (minutes < 1 && seconds < 1) {
        return false;
    }

    int total_seconds = minutes * 60 + seconds;
    if (total_seconds < 60) {
        total_seconds = 60;
    }

    active_ = true;
    remaining_seconds_ = total_seconds;
    user_away_ = false;
    away_streak_ = 0;
    present_streak_ = 0;
    reminder_cooldown_ = 0;
    presence_baseline_.clear();

    CalibratePresenceBaseline();
    WaitForPresenceMonitorExit();
    StartPresenceMonitor();
    ESP_LOGI(TAG, "Focus session started: %d seconds", total_seconds);
    return true;
}

bool FocusController::Stop() {
    if (!active_) {
        return true;
    }

    active_ = false;
    remaining_seconds_ = 0;
    user_away_ = false;
    away_streak_ = 0;
    present_streak_ = 0;
    presence_baseline_.clear();
    ESP_LOGI(TAG, "Focus session stopped");
    return true;
}

void FocusController::OnClockTick() {
    if (!active_) {
        return;
    }

    remaining_seconds_--;
    if (remaining_seconds_ <= 0) {
        OnFocusComplete();
        return;
    }

    if (reminder_cooldown_ > 0) {
        reminder_cooldown_--;
    } else if (user_away_) {
        reminder_cooldown_ = kReminderCooldownSec;
        auto& app = Application::GetInstance();
        app.Schedule([]() {
            Application::GetInstance().Alert(
                "Focus time!",
                "Still away — get back to your desk and lock in!",
                "angry",
                Lang::Sounds::OGG_POPUP);
        });
    }
}

void FocusController::WaitForPresenceMonitorExit() {
    while (presence_monitor_task_ != nullptr) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void FocusController::StartPresenceMonitor() {
    if (presence_monitor_task_ != nullptr) {
        return;
    }

    xTaskCreate(
        [](void* arg) {
            auto* self = static_cast<FocusController*>(arg);
            while (self->active_) {
                vTaskDelay(pdMS_TO_TICKS(kPresenceCheckIntervalSec * 1000));
                if (!self->active_) {
                    break;
                }
                self->CheckPresence();
            }
            self->presence_monitor_task_ = nullptr;
            vTaskDelete(nullptr);
        },
        "focus_presence",
        4096,
        this,
        2,
        &presence_monitor_task_);
}

void FocusController::CalibratePresenceBaseline() {
    auto* camera = Board::GetInstance().GetCamera();
    if (camera == nullptr) {
        ESP_LOGW(TAG, "No camera available for presence baseline");
        return;
    }

    if (camera->BuildPresenceGrid(presence_baseline_,
                                  focus_presence::kGridCols,
                                  focus_presence::kGridRows)) {
        ESP_LOGI(TAG, "Presence baseline calibrated");
    } else {
        ESP_LOGW(TAG, "Failed to calibrate presence baseline");
        presence_baseline_.clear();
    }
}

void FocusController::CheckPresence() {
    if (presence_baseline_.empty()) {
        return;
    }

    auto* camera = Board::GetInstance().GetCamera();
    if (camera == nullptr) {
        return;
    }

    std::vector<uint8_t> current;
    if (!camera->BuildPresenceGrid(current, focus_presence::kGridCols, focus_presence::kGridRows)) {
        ESP_LOGW(TAG, "Presence check capture failed");
        return;
    }

    float diff = focus_presence::CompareGrids(presence_baseline_, current);
    ESP_LOGD(TAG, "Presence scene diff: %.1f", diff);

    if (diff >= kAwayThreshold) {
        away_streak_++;
        present_streak_ = 0;
        if (!user_away_ && away_streak_ >= kAwayStreakRequired) {
            OnUserLeftDesk();
        }
    } else if (diff <= kPresentThreshold) {
        present_streak_++;
        away_streak_ = 0;
        if (user_away_ && present_streak_ >= kPresentStreakRequired) {
            user_away_ = false;
            away_streak_ = 0;
            present_streak_ = 0;
            ESP_LOGI(TAG, "User returned to desk");
        }
    } else {
        away_streak_ = 0;
        present_streak_ = 0;
    }
}

void FocusController::OnUserLeftDesk() {
    user_away_ = true;
    ESP_LOGI(TAG, "User left desk during focus");

    if (reminder_cooldown_ > 0) {
        return;
    }
    reminder_cooldown_ = kReminderCooldownSec;

    auto& app = Application::GetInstance();
    app.Schedule([]() {
        Application::GetInstance().Alert(
            "Focus time!",
            "Hey, you're in focus time — sit down and lock in!",
            "angry",
            Lang::Sounds::OGG_POPUP);
    });
}

void FocusController::OnFocusComplete() {
    active_ = false;
    remaining_seconds_ = 0;
    user_away_ = false;
    presence_baseline_.clear();

    auto& app = Application::GetInstance();
    app.Schedule([]() {
        Application::GetInstance().Alert(
            "Focus done!",
            "Great work. Time for a break.",
            "happy",
            Lang::Sounds::OGG_SUCCESS);
    });
    ESP_LOGI(TAG, "Focus session completed");
}

std::string FocusController::GetStatusJson() const {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "active", active_);
    cJSON_AddNumberToObject(root, "remaining_seconds", remaining_seconds_);
    cJSON_AddBoolToObject(root, "user_away", user_away_);
    cJSON_AddBoolToObject(root, "presence_monitoring", !presence_baseline_.empty());
    if (active_ && remaining_seconds_ > 0) {
        cJSON_AddNumberToObject(root, "remaining_minutes", (remaining_seconds_ + 59) / 60);
    }
    char* json = cJSON_PrintUnformatted(root);
    std::string result(json);
    cJSON_free(json);
    cJSON_Delete(root);
    return result;
}

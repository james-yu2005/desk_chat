#include "desk_dashboard.h"

#include "application.h"
#include "board.h"
#include "display/lcd_display.h"
#include "focus_controller.h"
#include "settings.h"

#include <esp_log.h>
#include <ctime>

static const char* TAG = "DeskDashboard";

static const char* kPetEmotions[] = {"neutral", "relaxed", "sleepy", "thinking", "idle"};
static const int kPetEmotionCount = sizeof(kPetEmotions) / sizeof(kPetEmotions[0]);

DeskDashboard& DeskDashboard::GetInstance() {
    static DeskDashboard instance;
    return instance;
}

void DeskDashboard::Initialize() {
    LoadFromSettings();

    auto& mcp_server = McpServer::GetInstance();

    mcp_server.AddTool(
        "self.dashboard.set_event",
        "Set the calendar / next-event line shown on the idle dashboard "
        "(e.g. 'Standup in 8 min' or 'Design review at 2pm').",
        PropertyList({
            Property("text", kPropertyTypeString)
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            SetEventLine(properties["text"].value<std::string>());
            return true;
        });

    mcp_server.AddTool(
        "self.dashboard.set_note",
        "Set a secondary note on the idle dashboard (watchlist summary, weather, etc.).",
        PropertyList({
            Property("text", kPropertyTypeString)
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            SetNoteLine(properties["text"].value<std::string>());
            return true;
        });

    mcp_server.AddTool(
        "self.dashboard.get_status",
        "Get the current idle dashboard lines (event and note).",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            (void)properties;
            return std::string("{\"event\":\"") + event_line_ + "\",\"note\":\"" + note_line_ + "\"}";
        });

    ESP_LOGI(TAG, "DeskDashboard initialized");
}

void DeskDashboard::LoadFromSettings() {
    Settings settings("desk");
    event_line_ = settings.GetString("event_line");
    note_line_ = settings.GetString("note_line");
}

void DeskDashboard::SaveEventLine() {
    Settings settings("desk", true);
    settings.SetString("event_line", event_line_);
}

void DeskDashboard::SaveNoteLine() {
    Settings settings("desk", true);
    settings.SetString("note_line", note_line_);
}

void DeskDashboard::SetEventLine(const std::string& text) {
    event_line_ = text;
    SaveEventLine();
    ESP_LOGI(TAG, "Event line: %s", event_line_.c_str());
}

void DeskDashboard::SetNoteLine(const std::string& text) {
    note_line_ = text;
    SaveNoteLine();
    ESP_LOGI(TAG, "Note line: %s", note_line_.c_str());
}

void DeskDashboard::OnClockTick(int clock_ticks) {
    if (Application::GetInstance().GetDeviceState() != kDeviceStateIdle) {
        return;
    }
    if (FocusController::GetInstance().IsActive()) {
        return;
    }

    auto display = Board::GetInstance().GetDisplay();
    auto* lcd = dynamic_cast<LcdDisplay*>(display);
    if (lcd == nullptr) {
        return;
    }

    lcd->UpdateIdleDashboard(clock_ticks, event_line_, note_line_);

    // Otto idle pet: shift emotion and position every 15 seconds.
    pet_tick_++;
    if (pet_tick_ >= 15) {
        pet_tick_ = 0;
        int emotion_index = (clock_ticks / 15) % kPetEmotionCount;
        display->SetEmotion(kPetEmotions[emotion_index]);

        int offset_x = ((clock_ticks / 15) % 5) - 2;
        int offset_y = ((clock_ticks / 30) % 3) - 1;
        lcd->SetOttoPetOffset(offset_x * 4, offset_y * 4);
    }
}

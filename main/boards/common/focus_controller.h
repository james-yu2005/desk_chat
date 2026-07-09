#ifndef FOCUS_CONTROLLER_H
#define FOCUS_CONTROLLER_H

#include "mcp_server.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <cstdint>
#include <string>
#include <vector>

// Pomodoro / focus mode with camera desk-presence monitoring.
class FocusController {
public:
    static FocusController& GetInstance();

    void Initialize();

    bool IsActive() const { return active_; }

    void OnClockTick();

    bool Start(int minutes, int seconds = 0);
    bool Stop();

    std::string GetStatusJson() const;

private:
    FocusController() = default;

    void OnFocusComplete();
    void CalibratePresenceBaseline();
    void CheckPresence();
    void OnUserLeftDesk();
    void StartPresenceMonitor();
    void WaitForPresenceMonitorExit();

    bool active_ = false;
    int remaining_seconds_ = 0;

    TaskHandle_t presence_monitor_task_ = nullptr;

    std::vector<uint8_t> presence_baseline_;
    bool user_away_ = false;
    int away_streak_ = 0;
    int present_streak_ = 0;
    int reminder_cooldown_ = 0;

    static constexpr int kPresenceCheckIntervalSec = 8;
    static constexpr int kAwayStreakRequired = 2;
    static constexpr int kPresentStreakRequired = 2;
    static constexpr float kAwayThreshold = 18.0f;
    static constexpr float kPresentThreshold = 12.0f;
    static constexpr int kReminderCooldownSec = 30;
};

#endif  // FOCUS_CONTROLLER_H

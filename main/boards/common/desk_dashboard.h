#ifndef DESK_DASHBOARD_H
#define DESK_DASHBOARD_H

#include "mcp_server.h"

#include <string>

// Idle dashboard data and MCP tools for calendar/event lines and notes.
class DeskDashboard {
public:
    static DeskDashboard& GetInstance();

    void Initialize();

    const std::string& GetEventLine() const { return event_line_; }
    const std::string& GetNoteLine() const { return note_line_; }

    void SetEventLine(const std::string& text);
    void SetNoteLine(const std::string& text);

    // Called every second from the clock tick when idle.
    void OnClockTick(int clock_ticks);

private:
    DeskDashboard() = default;

    void LoadFromSettings();
    void SaveEventLine();
    void SaveNoteLine();

    std::string event_line_;
    std::string note_line_;
    int pet_tick_ = 0;
};

#endif // DESK_DASHBOARD_H

#pragma once
#include "EventManager.h"
#include <Windows.h>
#include <string>

// Manages window position, size, and monitor persistence
class WindowStateManager : public IEventListener {
public:
    struct WindowState {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        std::wstring monitorDeviceName;
        int opacity = 255;
    };

    static WindowStateManager& getInstance();

    WindowState loadState();
    void saveWindowState(HWND hwnd);
    void restoreWindowState(HWND hwnd);

    // Snapping functionality
    void applySnapping(HWND hwnd);
    bool isNearEdge(HWND hwnd, int threshold);

    void onEvent(const Event& event) override;

private:
    WindowStateManager() = default;
    void saveState(const WindowState& state);
    bool isPositionValid(const WindowState& state);
    std::wstring getCurrentMonitorDeviceName(HWND hwnd);
};

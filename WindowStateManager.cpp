#include "WindowStateManager.h"
#include "Configuration.h"
#include "dist/json/json.h"
#include <fstream>

#define WINDOW_STATE_FILE_NAME "WindowState.json"

using namespace std;

WindowStateManager& WindowStateManager::getInstance() {
    static WindowStateManager instance;
    return instance;
}

WindowStateManager::WindowState WindowStateManager::loadState() {
    WindowState state;

    ifstream file(WINDOW_STATE_FILE_NAME);
    if (!file.good()) {
        // No saved state, return defaults
        state.width = Config::WindowDefaults::DEFAULT_WIDTH;
        state.height = Config::WindowDefaults::DEFAULT_HEIGHT;
        state.opacity = Config::OpacityConfig::DEFAULT_OPACITY;
        return state;
    }

    Json::Value json;
    Json::Reader reader;
    reader.parse(file, json);

    if (json["x"].isInt()) state.x = json["x"].asInt();
    if (json["y"].isInt()) state.y = json["y"].asInt();
    if (json["width"].isInt()) state.width = json["width"].asInt();
    if (json["height"].isInt()) state.height = json["height"].asInt();
    if (json["opacity"].isInt()) state.opacity = json["opacity"].asInt();

    if (json["monitor"].isString()) {
        string monitorStr = json["monitor"].asString();
        state.monitorDeviceName = wstring(monitorStr.begin(), monitorStr.end());
    }

    // Validate dimensions
    if (state.width <= 0 || state.height <= 0) {
        state.width = Config::WindowDefaults::DEFAULT_WIDTH;
        state.height = Config::WindowDefaults::DEFAULT_HEIGHT;
    }

    return state;
}

void WindowStateManager::saveWindowState(HWND hwnd) {
    if (!hwnd) return;

    WindowState state;
    RECT rect;
    GetWindowRect(hwnd, &rect);

    state.x = rect.left;
    state.y = rect.top;
    state.width = rect.right - rect.left;
    state.height = rect.bottom - rect.top;
    state.monitorDeviceName = getCurrentMonitorDeviceName(hwnd);

    saveState(state);
}

void WindowStateManager::saveState(const WindowState& state) {
    Json::Value json;
    json["x"] = state.x;
    json["y"] = state.y;
    json["width"] = state.width;
    json["height"] = state.height;
    json["opacity"] = state.opacity;

    // Convert wstring to string for JSON
    string monitorStr(state.monitorDeviceName.begin(), state.monitorDeviceName.end());
    json["monitor"] = monitorStr;

    // Write to file
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream outputFileStream(WINDOW_STATE_FILE_NAME);
    writer->write(json, &outputFileStream);
}

void WindowStateManager::restoreWindowState(HWND hwnd) {
    if (!hwnd) return;

    WindowState state = loadState();

    if (!isPositionValid(state)) {
        // Position is invalid (off-screen), center on primary monitor
        RECT workArea;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
        state.x = (workArea.right - workArea.left - state.width) / 2;
        state.y = (workArea.bottom - workArea.top - state.height) / 2;
    }

    SetWindowPos(hwnd, nullptr, state.x, state.y, state.width, state.height,
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

bool WindowStateManager::isPositionValid(const WindowState& state) {
    // Check if position is on any monitor
    RECT testRect;
    testRect.left = state.x;
    testRect.top = state.y;
    testRect.right = state.x + state.width;
    testRect.bottom = state.y + state.height;

    HMONITOR monitor = MonitorFromRect(&testRect, MONITOR_DEFAULTTONULL);
    return monitor != nullptr;
}

std::wstring WindowStateManager::getCurrentMonitorDeviceName(HWND hwnd) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEXW info;
    info.cbSize = sizeof(MONITORINFOEXW);

    if (GetMonitorInfoW(monitor, &info)) {
        return std::wstring(info.szDevice);
    }

    return L"";
}

void WindowStateManager::applySnapping(HWND hwnd) {
    if (!Config::SnapConfig::ENABLE_SNAP) return;
    if (!hwnd) return;

    RECT windowRect, workArea;
    GetWindowRect(hwnd, &windowRect);
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

    int threshold = Config::SnapConfig::SNAP_THRESHOLD;
    bool snapped = false;
    RECT newRect = windowRect;

    // Check left edge
    if (abs(windowRect.left - workArea.left) < threshold) {
        newRect.left = workArea.left;
        newRect.right = newRect.left + (windowRect.right - windowRect.left);
        snapped = true;
    }

    // Check right edge
    if (abs(windowRect.right - workArea.right) < threshold) {
        newRect.right = workArea.right;
        newRect.left = newRect.right - (windowRect.right - windowRect.left);
        snapped = true;
    }

    // Check top edge
    if (abs(windowRect.top - workArea.top) < threshold) {
        newRect.top = workArea.top;
        newRect.bottom = newRect.top + (windowRect.bottom - windowRect.top);
        snapped = true;
    }

    // Check bottom edge
    if (abs(windowRect.bottom - workArea.bottom) < threshold) {
        newRect.bottom = workArea.bottom;
        newRect.top = newRect.bottom - (windowRect.bottom - windowRect.top);
        snapped = true;
    }

    if (snapped) {
        SetWindowPos(hwnd, nullptr, newRect.left, newRect.top,
                    newRect.right - newRect.left, newRect.bottom - newRect.top,
                    SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

bool WindowStateManager::isNearEdge(HWND hwnd, int threshold) {
    if (!hwnd) return false;

    RECT windowRect, workArea;
    GetWindowRect(hwnd, &windowRect);
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

    return (abs(windowRect.left - workArea.left) < threshold ||
            abs(windowRect.right - workArea.right) < threshold ||
            abs(windowRect.top - workArea.top) < threshold ||
            abs(windowRect.bottom - workArea.bottom) < threshold);
}

void WindowStateManager::onEvent(const Event& event) {
    // Can handle WindowMoved events if needed
}

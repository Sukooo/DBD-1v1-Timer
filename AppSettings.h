#pragma once
#include <nlohmann/json.hpp>

// Forward declare controller constants (defined in ControllerManager.h)
// We use hardcoded values here to avoid circular dependencies
constexpr unsigned short CONTROLLER_A_DEFAULT = 5010;
constexpr unsigned short CONTROLLER_B_DEFAULT = 5011;
constexpr unsigned short CONTROLLER_LEFT_DEFAULT = 5003;
constexpr unsigned short CONTROLLER_RIGHT_DEFAULT = 5002;

// Color settings
struct ColorSettings {
    int timerColor = 9;
    int selectedTimerColor = 6;
    int lastSecondsColor = 1;
    int backgroundColor = 20;
};

// Main application settings - modern struct-based design with public members
struct AppSettings {
    // Keyboard hotkeys
    int startKey = 70;  // F key
    int timer1Key = 112;  // P key
    int timer2Key = 113;  // Q key
    int startNoResetKey = 72;  // H key
    int hotkeyClickThrough = 0;
    int hotkeyOpacityUp = 0;
    int hotkeyOpacityDown = 0;

    // Controller hotkeys
    int conStartKey = CONTROLLER_A_DEFAULT;
    int conTimer1Key = CONTROLLER_LEFT_DEFAULT;
    int conTimer2Key = CONTROLLER_RIGHT_DEFAULT;
    int conStartNoResetKey = CONTROLLER_B_DEFAULT;

    // Options
    bool optionStartOnChange = false;
    bool optionTransparent = false;
    bool optionClickThrough = false;
    int opacity = 255;

    // Colors
    ColorSettings colors;
};

// JSON serialization - nlohmann/json automatic serialization
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ColorSettings,
    timerColor, selectedTimerColor, lastSecondsColor, backgroundColor)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AppSettings,
    startKey, timer1Key, timer2Key, startNoResetKey,
    hotkeyClickThrough, hotkeyOpacityUp, hotkeyOpacityDown,
    conStartKey, conTimer1Key, conTimer2Key, conStartNoResetKey,
    optionStartOnChange, optionTransparent, optionClickThrough,
    opacity, colors)

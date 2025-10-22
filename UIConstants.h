#pragma once
#include <Windows.h>

// HWND Control IDs
constexpr byte CID_OK = 100;
constexpr byte CID_CANCEL = 101;
constexpr byte CID_START = 103;
constexpr byte CID_TIMER1 = 104;
constexpr byte CID_TIMER2 = 105;
constexpr byte CID_START_NO_RESET = 116;
constexpr byte CID_TRANSPARENT_CB = 106;
constexpr byte CID_CLICKTHROUGH_CB = 107;
constexpr byte CID_STARTONCHANGE_CB = 112;
constexpr byte CID_CON_START = 113;
constexpr byte CID_CON_TIMER1 = 114;
constexpr byte CID_CON_TIMER2 = 115;
constexpr byte CID_CON_START_NO_RESET = 117;
constexpr byte CID_CLICKTHROUGH_HOTKEY = 118;
constexpr byte CID_OPACITY_SLIDER = 119;
constexpr byte CID_OPACITY_UP_HOTKEY = 120;
constexpr byte CID_OPACITY_DOWN_HOTKEY = 121;

// HWND Color Control IDs
constexpr byte CID_TIMER_COLOR = 108;
constexpr byte CID_SELECTED_TIMER_COLOR = 109;
constexpr byte CID_LAST_SECONDS_COLOR = 110;
constexpr byte CID_BACKGROUND_COLOR = 111;
constexpr byte CID_COLOR_PREVIEW = 25;

// Global Sizes
constexpr UINT16 SIZE_SETTINGS_WIDTH = 400;
constexpr UINT16 SIZE_SETTINGS_HEIGHT = 660;
constexpr UINT16 SIZE_COLORPICKER_WIDTH = 270;
constexpr UINT16 SIZE_COLORPICKER_HEIGHT = 350;

// Bitmaps
constexpr byte IDB_MOUSE = 110;
constexpr byte IDB_CONTROLLER = 111;

// Menu IDs
constexpr byte MENU_QUIT = 1;
constexpr byte MENU_SETTINGS = 0;

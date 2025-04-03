#pragma once
#include <string>
#include <Windows.h>

// HWND Control IDs
constexpr byte CID_OK = 100;
constexpr byte CID_CANCEL = 101;
constexpr byte CID_START = 103;
constexpr byte CID_TIMER1 = 104;
constexpr byte CID_TIMER2 = 105;
constexpr byte CID_TRANSPARENT_CB = 106;
constexpr byte CID_CLICKTHROUGH = 107;
constexpr byte CID_STARTONCHANGE_CB = 112;
constexpr byte MENU_QUIT = 1;
constexpr byte MENU_SETTINGS = 0;
constexpr byte KEY_START = 0;
constexpr byte KEY_TIMER1 = 1;
constexpr byte KEY_TIMER2 = 2;
constexpr byte OPTION_TRANSPARENT = 3;
constexpr byte OPTION_CLICKTHROUGH = 4;

// HWND Color Control IDs
constexpr byte CID_TIMER_COLOR = 108;
constexpr byte CID_SELECTED_TIMER_COLOR = 109;
constexpr byte CID_LAST_SECONDS_COLOR = 110;
constexpr byte CID_BACKGROUND_COLOR = 111;
constexpr byte CID_COLOR_PREVIEW = 25;

// Global Sizes
constexpr UINT16 SIZE_SETTINGS_WIDTH = 400;
constexpr UINT16 SIZE_SETTINGS_HEIGHT = 630;
constexpr UINT16 SIZE_COLORPICKER_WIDTH = 270;
constexpr UINT16 SIZE_COLORPICKER_HEIGHT = 350;

// Bitmaps
constexpr byte IDB_MOUSE = 110;
constexpr byte IDB_CONTROLLER = 111;

// Custom HWND messages
constexpr int REFRESH_BRUSHES(WM_APP + 1);
constexpr int HOTKEY_HIT(WM_APP + 2);

// Configuration File Names
#define SETTINGS_FILE_NAME "Settings.json"

// Structs
struct ColorsStruct // With default values
{
	int timerColor = 9;
	int selectedTimerColor = 6;
	int lastSecondsColor = 1;
	int backgroundColor = 20;
};

struct SettingsStruct // With default values
{
	int startKey = 70;
	int timer1Key = 112;
	int timer2Key = 113;
	bool optionStartOnChange = false;
	bool optionTransparent = false;
	bool clickthrough = false;
	ColorsStruct colors;
};

// Variables
extern HINSTANCE hInstanceGlobal;
extern HBRUSH hBrushes[25];
extern SettingsStruct appSettings;
extern HWND hwndMainWindow;

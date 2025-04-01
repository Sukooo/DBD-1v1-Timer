#pragma once
#include <Windows.h>

// HWND Control IDs
constexpr byte CID_OK = 100;
#define CID_CANCEL					101
#define CID_START					103
#define CID_TIMER1					104
#define CID_TIMER2					105
#define CID_TRANSPARENT_CB			106
#define CID_CLICKTHROUGH			107
#define MENU_QUIT					1
#define MENU_SETTINGS				0
#define KEY_START					0
#define KEY_TIMER1					1
#define KEY_TIMER2					2
#define OPTION_TRANSPARENT			3
#define OPTION_CLICKTHROUGH			4

// HWND Color Control IDs
#define COLOR_CTR_TIMER				108
#define COLOR_CTR_SELECTED_TIMER	109
#define COLOR_CTR_LAST_SECONDS		110
#define COLOR_CTR_BACKGROUND		111
#define COLOR_PREVIEW				25

// Global Sizes
#define SIZE_SETTINGS_WIDTH			400
#define SIZE_SETTINGS_HEIGHT		590
#define SIZE_COLORPICKER_WIDTH		270
#define SIZE_COLORPICKER_HEIGHT		350

// Bitmaps
#define IDB_MOUSE					110
#define IDB_CONTROLLER				111

// Custom HWND messages
#define REFRESH_BRUSHES				(WM_APP + 1)

// Configuration File Names
#define SETTINGS_FILE_NAME			"Settings.json"

// Structs
struct ColorsStruct
{
	int timerColor;
	int selectedTimerColor;
	int lastSecondsColor;
	int backgroundColor;
};

struct SettingsStruct
{
	int startKey;
	int timer1Key;
	int timer2Key;
	bool optionTransparent;
	bool clickthrough;
	ColorsStruct colors;
};

// Variables
extern HINSTANCE hInstanceGlobal;
extern HBRUSH hBrushes[25];
extern SettingsStruct appSettings;
extern HWND hwndMainWindow;

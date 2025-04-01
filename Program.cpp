#include <windows.h>
#include <d2d1.h>
#include <string>
#include <thread>
#include <dwrite.h>
#include <commctrl.h>
#include "Globals.h"
#include "helperFunctions.h"
#include "BaseWindow.h"
#include "Timer.h"
#include "SettingsWindow.h"
#include "ColorPickerWindow.h"
#include "MainWindow.h"
#include "Program.h"

#pragma comment(lib, "Msimg32.lib")
#pragma comment (lib, "d2d1")

using std::thread; using std::wstring;

// Global variable assignment (defined in MainWindow.h)
SettingsStruct appSettings;
HBRUSH hBrushes[25];
HWND hwndMainWindow = nullptr;
HINSTANCE hInstanceGlobal;
MainWindow* pGlobalTimerWindow = NULL; // used for the hook procedure

void appLoop(MainWindow* win)
{
	while (win->appRunning)
	{
		Sleep(1);
		win->timer1.updateTime();
		win->timer2.updateTime();
		win->draw();
	}
}

void exitApp()
{
	if (pGlobalTimerWindow) pGlobalTimerWindow->appRunning = false;
	PostQuitMessage(0);
}

LRESULT CALLBACK kbHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_KEYDOWN && pGlobalTimerWindow != NULL && appSettings.startKey != NULL)
	{
		KBDLLHOOKSTRUCT* pKbdHookStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
		const int startKey = appSettings.startKey;
		int hitKey = pKbdHookStruct->vkCode;

		// If the hit key is Alt or Control, set it to not matter if it was right or left.
		if (hitKey == VK_LMENU || hitKey == VK_RMENU) {
			hitKey = VK_MENU;
		}
		else if (hitKey == VK_LCONTROL || hitKey == VK_RCONTROL) {
			hitKey = VK_CONTROL;
		}
		
		// Take action according to hit hotkey
		if (hitKey == appSettings.startKey)
		{
			pGlobalTimerWindow->handleHotKey(KEY_START);
		}
		else if (hitKey == appSettings.timer1Key)
		{
			pGlobalTimerWindow->handleHotKey(KEY_TIMER1);
		}
		else if (hitKey == appSettings.timer2Key)
		{
			pGlobalTimerWindow->handleHotKey(KEY_TIMER2);
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Create the main window
	MainWindow win;

	try
	{
		// Global hInstance variable (declared in globals.h)
		hInstanceGlobal = hInstance;

		// Create settings file on program first run
		if (!settingsFileExists())
		{
			createSettingsFile();
		}

		// Initiate common controls lib
		InitCommonControls();

		// Initialize brushes
		initializeBrushes();

		if (!win.create(L"Timer", 0, 0, 285, 40, WS_EX_TOPMOST | WS_EX_LAYERED, WS_POPUP)) {
			return 0;
		}
		SetLayeredWindowAttributes(win.window(), 1, 255, LWA_COLORKEY | LWA_ALPHA);

		ShowWindow(win.window(), nCmdShow);

		// Create variables for settings and color picker windows
		SettingsWindow settings;
		ColorPickerWindow colorPicker;
		settings.pColorPicker = &colorPicker;

		win.pSettingsWindow = &settings;

		// global variables for timer
		pGlobalTimerWindow = &win;
		hwndMainWindow = win.window();

		// Apply saved settings
		applySettings(appSettings);

		// Listen for keys: F1, F2, F While running in the background - Install a hook procedure
		HHOOK kbd = SetWindowsHookEx(WH_KEYBOARD_LL, &kbHook, 0, 0);

		// Create a thread for the app loop (ticks)
		thread t1(appLoop, &win);

		// Handle messages
		MSG msg = { };
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		t1.join();
		return 0;
	}
	catch (const std::exception& e)
	{
		exitApp();
	}
}
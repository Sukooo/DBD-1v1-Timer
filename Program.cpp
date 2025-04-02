#include <windows.h>
#include <d2d1.h>
#include <string>
#include <thread>
#include <dwrite.h>
#include <commctrl.h>
#include "Globals.h"
#include "ResourceUtils.h"
#include "SettingsUtils.h"
#include "BaseWindow.h"
#include "Timer.h"
#include "SettingsWindow.h"
#include "ColorPickerWindow.h"
#include "MainWindow.h"
#include "Program.h"

#include "HotkeyManager.h"

#pragma comment(lib, "Msimg32.lib")
#pragma comment (lib, "d2d1")

using std::thread; using std::wstring;

// Global variable assignment (defined in MainWindow.h)
SettingsStruct appSettings;
HBRUSH hBrushes[25];
HWND hwndMainWindow = nullptr;
HINSTANCE hInstanceGlobal;
MainWindow* pGlobalTimerWindow = nullptr;

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

LRESULT CALLBACK mouseHook(const int nCode, const WPARAM wParam, const LPARAM lParam)
{
	const MSLLHOOKSTRUCT* pMsHookStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
	int key = 0;

	switch (wParam)
	{
	case WM_LBUTTONDOWN:
		key = VK_LBUTTON;
		break;
	case WM_RBUTTONDOWN:
		key = VK_RBUTTON;
		break;
	case WM_MBUTTONDOWN:
		key = VK_MBUTTON;
		break;
	case WM_XBUTTONDOWN:
		key = HIWORD(pMsHookStruct->mouseData) == 1 ? VK_XBUTTON1 : VK_XBUTTON2;
		break;
	default:
		break;
	}

	if (key != 0)
	{
		HotkeyManager::execute(key);
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK kbHook(const int nCode, const WPARAM wParam, const LPARAM lParam)
{
	if (wParam == WM_KEYDOWN && pGlobalTimerWindow != nullptr && appSettings.startKey != NULL)
	{
		const KBDLLHOOKSTRUCT* pKbdHookStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
		int hitKey = pKbdHookStruct->vkCode;

		// If the hit key is Alt, Control or Shift, set it to not matter if it was Right or Left.
		if (hitKey == VK_LMENU || hitKey == VK_RMENU) {
			hitKey = VK_MENU;
		}
		else if (hitKey == VK_LCONTROL || hitKey == VK_RCONTROL) {
			hitKey = VK_CONTROL;
		}
		else if (hitKey == VK_LSHIFT || hitKey == VK_RSHIFT)
		{
			hitKey = VK_SHIFT;
		}
		
		HotkeyManager::execute(hitKey);
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd)
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

		ShowWindow(win.window(), nShowCmd);

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

		// Listen for hotkeys While Running in the background - Install a hook procedure
		SetWindowsHookEx(WH_KEYBOARD_LL, &kbHook, nullptr, NULL);
		SetWindowsHookEx(WH_MOUSE_LL, &mouseHook, nullptr, NULL);

		// Create a thread for the app loop (ticks)
		thread t1(appLoop, &win);

		// Handle messages
		MSG msg = { };
		while (GetMessage(&msg, nullptr, 0, 0)) {
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
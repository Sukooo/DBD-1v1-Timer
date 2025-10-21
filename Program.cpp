#include <windows.h>
#include <d2d1.h>
#include <string>
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
#include "Configuration.h"
#include "SettingsManager.h"
#include "WindowStateManager.h"

#include "ControllerManager.h"
#include "HotkeyManager.h"

#pragma comment(lib, "Msimg32.lib")
#pragma comment (lib, "d2d1")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Xinput.lib")

using std::wstring;

// Global variable assignment (defined in MainWindow.h)
SettingsStruct appSettings;
HBRUSH hBrushes[25];
HWND hwndMainWindow = nullptr;
HINSTANCE hInstanceGlobal;
MainWindow* pGlobalTimerWindow = nullptr;

void exitApp()
{
	if (pGlobalTimerWindow) pGlobalTimerWindow->appRunning = false;
	PostQuitMessage(0);
}

LRESULT CALLBACK mouseHook(const int nCode, const WPARAM wParam, const LPARAM lParam)
{
	if (nCode >= 0)
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
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK kbHook(const int nCode, const WPARAM wParam, const LPARAM lParam)
{
	if (
		nCode >= 0 &&
		wParam == WM_KEYDOWN)
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

void controllerInputCallback(const WORD buttons)
{
	SendMessage(hwndMainWindow, CONTROLLER_INPUT ,buttons, NULL);
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
		if (!SettingsManager::getInstance().fileExists())
		{
			SettingsManager::getInstance().createDefaultFile();
		}

		// Initiate common controls lib
		InitCommonControls();

		// Initialize brushes
		initializeBrushes();
		
		// Load window state (position, size, monitor)
		WindowStateManager::WindowState savedState = WindowStateManager::getInstance().loadState();
		int x = savedState.x;
		int y = savedState.y;
		int width = savedState.width;
		int height = savedState.height;
		
		// Use defaults if no saved state (first run)
		if (width <= 0 || height <= 0) {
			x = 0;
			y = 0;
			width = Config::WindowDefaults::DEFAULT_WIDTH;
			height = Config::WindowDefaults::DEFAULT_HEIGHT;
		}

		if (!win.create(L"Timer", x, y, width, height, WS_EX_TOPMOST | WS_EX_LAYERED, WS_POPUP)) {
			return 0;
		}

		// Apply saved opacity from settings
		SettingsManager::getInstance().loadFromFile();
		const auto& settings = SettingsManager::getInstance().getSettings();
		SetLayeredWindowAttributes(win.window(), 0, settings.opacity, LWA_ALPHA);

		ShowWindow(win.window(), nShowCmd);
		
		// Validate and restore to correct monitor
		WindowStateManager::getInstance().restoreWindowState(win.window());

		// Create variables for settings and color picker windows
		SettingsWindow settingsWindow;
		ColorPickerWindow colorPicker;
		settingsWindow.pColorPicker = &colorPicker;

		win.pSettingsWindow = &settingsWindow;

		// global variables for timer
		pGlobalTimerWindow = &win;
		hwndMainWindow = win.window();

		// Apply saved settings (uses old SettingsUtils for backward compat)
		applySettings(appSettings);

		// Listen for hotkeys While Running in the background - Install a hook procedure
		SetWindowsHookEx(WH_KEYBOARD_LL, &kbHook, nullptr, NULL);
		SetWindowsHookEx(WH_MOUSE_LL, &mouseHook, nullptr, NULL);

		// Controller support
		std::unique_ptr<ControllerManager> controllerManager(std::make_unique<ControllerManager>());
		controllerManager->setInputCallback(controllerInputCallback);
		controllerManager->start();

		// Main message loop
		MSG msg = { };
		while (GetMessage(&msg, nullptr, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		controllerManager->stop();
		return 0;
	}
	catch (const std::exception& e)
	{
		exitApp();
	}
}

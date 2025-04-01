#include "SettingsWindow.h"
#include "HelperFunctions.h"
#include "BaseWindow.h"
#include "Program.h"
#include <CommCtrl.h>
#include <windowsx.h>
#include <exception>

// Methods

void SettingsWindow::initializeWindow()
{
	// retrieve settings
	SettingsWindow::tempSettings_ = getSafeSettingsStruct();

	// Set up text
	initializeTextControls();

	// Set up buttons and hotkeys
	initializeButtonControls();

	// Set up bitmaps
	initializeBitmaps();
}

void SettingsWindow::initializeBitmaps()
{
	mouseBitmap_ = loadBitmapResource(IDB_MOUSE);
	controllerBitmap_ = loadBitmapResource(IDB_CONTROLLER);
}

void SettingsWindow::displayBitmaps()
{
	// preset values
	int mouseX = SIZE_SETTINGS_WIDTH / 2 + 45; int mouseY = 35;
	int controllerX = mouseX + 75; int controllerY = 35;
	int bitmapSize = 25;
	int originalBitmapSize = 400;
	COLORREF ignoreColor = RGB(100, 100, 100);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd_, &ps);

	// Create a compatible device context
	HDC hMemDC = CreateCompatibleDC(hdc);

	// Select the mouse bitmap into the memory DC
	HGDIOBJ oldBitmap = SelectObject(hMemDC, mouseBitmap_);

	// Scale bitmaps
	TransparentBlt(hdc,
		mouseX, mouseY,			         // Target x, y coordinates
		bitmapSize,				         // Target width
		bitmapSize,						 // Target height
		hMemDC,
		0, 0,			                 // Source x, y coordinates
		originalBitmapSize,				 // Source width
		originalBitmapSize,				 // Source height
		ignoreColor);

	// Select the controller bitmap into the memory DC
	oldBitmap = SelectObject(hMemDC, controllerBitmap_);

	// Scale bitmaps
	TransparentBlt(hdc,
		controllerX, controllerY,			         // Target x, y coordinates
		bitmapSize,				         // Target width
		bitmapSize,						 // Target height
		hMemDC,
		0, 0,			                 // Source x, y coordinates
		originalBitmapSize,				 // Source width
		originalBitmapSize,				 // Source height
		ignoreColor);

	// Cleanup
	SelectObject(hMemDC, oldBitmap);
	DeleteDC(hMemDC);

	EndPaint(hwnd_, &ps);
}

void SettingsWindow::initializeTextControls()
{
	// preset values
	int xTitle = 15;
	int yOffset = 35;

	// Initialize headers
	HWND hwndTitleHotkeys = createControl(WC_STATIC, L"Hotkeys", SIZE_SETTINGS_WIDTH / 2 - 40, 5, 60, 40);
	HWND hwndTitleOptions = createControl(WC_STATIC, L"Options", SIZE_SETTINGS_WIDTH / 2 - 40, yOffset * 6 - 20, 80, 40);
	HWND hwndTitleColors = createControl(WC_STATIC, L"Colors", SIZE_SETTINGS_WIDTH / 2 - 40, yOffset * 10 - 20, 60, 40);

	// Hotkey titles
	HWND hwndTextStart = createControl(WC_STATIC, L"Start / Stop / Reset", xTitle, yOffset * 2, 150, 40);
	HWND hwndTextTimer1 = createControl(WC_STATIC, L"Timer 1", xTitle, yOffset * 3, 150, 20);
	HWND hwndTextTimer2 = createControl(WC_STATIC, L"Timer 2", xTitle, yOffset * 4, 150, 20);

	// Checkbox titles
	HWND hwndTextTransparentBackground = createControl(WC_STATIC, L"Transparent Background", xTitle, yOffset * 7 - 20, 150, 20);
	HWND hwndTextCheckboxClickthrough = createControl(WC_STATIC, L"Clickthrough (resets when app is closed)", xTitle, yOffset * 8 - 20, 150, 40);

	// Break lines
	HWND hwndBreakLine1 = createControl(WC_STATIC, NULL, 15, yOffset * 5, SIZE_SETTINGS_WIDTH - 40, 5, NULL, SS_ETCHEDHORZ);
	HWND hwndBreakLine2 = createControl(WC_STATIC, NULL, 15, yOffset * 9, SIZE_SETTINGS_WIDTH - 40, 5, NULL, SS_ETCHEDHORZ);

	// Color options names
	HWND hwndTextColorTimer = createControl(WC_STATIC, L"Timer", xTitle, yOffset * 11 - 20, 150, 40);
	HWND hwndTextColorSelectedTimer = createControl(WC_STATIC, L"Selected Timer", xTitle, yOffset * 12 - 20, 150, 40);
	HWND hwndTextColorWinCon = createControl(WC_STATIC, L"Last 20 Seconds", xTitle, yOffset * 13 - 20, 150, 40);
	HWND hwndTextColorBackground = createControl(WC_STATIC, L"Background", xTitle, yOffset * 14 - 20, 150, 40);

	// Copyright text
	HWND hwndCopyright = createControl(WC_STATIC, L"© Truueh 2024", 10, SIZE_SETTINGS_HEIGHT - 65, 100, 40);

	// Apply fonts
	setControlsFont(hwnd_);
	setTitleFont(hwndTitleColors);
	setTitleFont(hwndTitleHotkeys);
	setCopyrightFont(hwndCopyright);
}

void SettingsWindow::initializeButtonControls()
{
	// preset values
	int yOffset = 35;

	int xHotkey = SIZE_SETTINGS_WIDTH / 2 + 20;
	int xHotkeyCon = xHotkey + 80;
	int xColorButton = xHotkeyCon - 50;
	int widthHotkey = 70;
	int heightHotkey = 20;
	int sizeCheckbox = 40;
	int xCheckbox = SIZE_SETTINGS_WIDTH - 70;

	HWND hotkeys[6];
	HWND colorButtons[4];

	// Hotkeys
		// Mouse
	hotkeys[0] = createControl(WC_BUTTON, L"", xHotkey, yOffset * 2, widthHotkey, heightHotkey, CID_START, BS_FLAT); // Start key
	hotkeys[1] = createControl(WC_BUTTON, L"", xHotkey, yOffset * 3, widthHotkey, heightHotkey, CID_TIMER1, BS_FLAT); // Timer 1 key
	hotkeys[2] = createControl(WC_BUTTON, L"", xHotkey, yOffset * 4, widthHotkey, heightHotkey, CID_TIMER2, BS_FLAT); // Timer 2 key

		// Controller
	hotkeys[3] = createControl(WC_BUTTON, L"test", xHotkeyCon, yOffset * 2, widthHotkey, heightHotkey, 0, BS_FLAT); // Start key
	hotkeys[4] = createControl(WC_BUTTON, L"test", xHotkeyCon, yOffset * 3, widthHotkey, heightHotkey, 0, BS_FLAT); // Timer 1 key
	hotkeys[5] = createControl(WC_BUTTON, L"test", xHotkeyCon, yOffset * 4, widthHotkey, heightHotkey, 0, BS_FLAT); // Timer 2 key

	// Checkbox buttons
	HWND hCbTransparentBg = createControl(WC_BUTTON, L"", xCheckbox, yOffset * 7 - 30, sizeCheckbox, sizeCheckbox, CID_TRANSPARENT_CB, BS_CHECKBOX | BS_AUTOCHECKBOX);
	HWND hCbClickthrough = createControl(WC_BUTTON, L"", xCheckbox, yOffset * 8 - 30, sizeCheckbox, sizeCheckbox, CID_CLICKTHROUGH, BS_CHECKBOX | BS_AUTOCHECKBOX);

	// Color buttons
	colorButtons[0] = createControl(WC_BUTTON, L"", xColorButton, yOffset * 11 - 20, 100, 15, CID_TIMER_COLOR, BS_OWNERDRAW);
	colorButtons[1] = createControl(WC_BUTTON, L"", xColorButton, yOffset * 12 - 20, 100, 15, CID_SELECTED_TIMER_COLOR, BS_OWNERDRAW);
	colorButtons[2] = createControl(WC_BUTTON, L"", xColorButton, yOffset * 13 - 20, 100, 15, CID_LAST_SECONDS_COLOR, BS_OWNERDRAW);
	colorButtons[3] = createControl(WC_BUTTON, L"", xColorButton, yOffset * 14 - 20, 100, 15, CID_BACKGROUND_COLOR, BS_OWNERDRAW);

	// Initialize exit controls
	HWND hwndOKButton = createControl(WC_BUTTON, L"OK", SIZE_SETTINGS_WIDTH - 160, SIZE_SETTINGS_HEIGHT - 80, 50, 25, CID_OK);
	HWND hwndCancelButton = createControl(WC_BUTTON, L"CANCEL", SIZE_SETTINGS_WIDTH - 100, SIZE_SETTINGS_HEIGHT - 80, 70, 25, CID_CANCEL);

	// Apply font
	setControlsFont(hwnd_);
	
	// Apply currently set hotkeys
	for (HWND hCtrl : hotkeys) {
		applyHotkeySavedKey(hCtrl);
	}

	// Apply currently set options
	SendMessage(hCbTransparentBg, BM_SETCHECK, appSettings.optionTransparent, 0);
	SendMessage(hCbClickthrough, BM_SETCHECK, appSettings.clickthrough, 0);
}

HWND SettingsWindow::createControl(LPCWSTR className, LPCWSTR controlName, int x, int y, int width, int height, int id, long ADDITIONAL_STYLE) {
	return CreateWindowEx(0, className, controlName, WS_VISIBLE | WS_CHILDWINDOW | ADDITIONAL_STYLE, x, y, width, height, hwnd_, (HMENU)id, NULL, NULL);
}

void SettingsWindow::setCopyrightFont(HWND hControl)
{
	HFONT hFont = CreateFont(
		14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial"
	);

	SendMessage(hControl, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void SettingsWindow::handleControlCommand(LPARAM lParam)
{
	HWND hwndCtrl = reinterpret_cast<HWND>(lParam); // clicked item handle
	int controlId = GetDlgCtrlID(hwndCtrl); // retrieve control ID

	// Clicked control
	switch (controlId) {
	// OK
	case CID_OK:
		applySettings(tempSettings_);
		SendMessage(GetWindow(hwnd_, GW_OWNER), REFRESH_BRUSHES, 0, 0);
		DestroyWindow(hwnd_);
		break;
		
	// Cancel
	case CID_CANCEL:
		DestroyWindow(hwnd_);
		break;

	// Any hotkey control clicked
	case CID_START:
	case CID_TIMER1:
	case CID_TIMER2:
	{
		if (hActiveControl_) { // Reset currently selected control (before this control was clicked)
			applyHotkeySavedKey(hActiveControl_);
			hActiveControl_ = nullptr;
		}

		SetFocus(hwnd_);
		hActiveControl_ = hwndCtrl;
		SetWindowText(hActiveControl_, L"...");
		break;
	}

	// Transparent background checkbox
	case CID_TRANSPARENT_CB: 
		tempSettings_.optionTransparent = (Button_GetCheck(hwndCtrl) == BST_CHECKED);
		break;

	// Clickthrough checkbox
	case CID_CLICKTHROUGH:
		tempSettings_.clickthrough = (Button_GetCheck(hwndCtrl) == BST_CHECKED);
		break;

	// Any color control clicked
	case CID_TIMER_COLOR:
	case CID_SELECTED_TIMER_COLOR:
	case CID_LAST_SECONDS_COLOR:
	case CID_BACKGROUND_COLOR:
	{
		// Open a Color Picker window
		if (pColorPicker->window() == NULL)
		{
			pColorPicker->controlId = controlId; // Notify Color Picker who called it
			pColorPicker->pTempSettings = &tempSettings_;
		
			if (!pColorPicker->create(L"Color Picker", 850, 300, SIZE_COLORPICKER_WIDTH, SIZE_COLORPICKER_HEIGHT, 0, WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX, hwnd_, 0, NULL)) {
				return;
			}
			ShowWindow(pColorPicker->window(), SW_SHOW);
		}
		else
		{
			SetForegroundWindow(pColorPicker->window());
		}
	}
	break;
	}
}

void SettingsWindow::colorHandles(LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
	// In case of invalid color index (to prevent index out of range)
	if (tempSettings_.colors.timerColor > 24) {
		FillRect(pDIS->hDC, &pDIS->rcItem, hBrushes[0]);
		return;
	}

	// Apply saved color to the color controls in the setting's window
	switch (pDIS->CtlID)
	{
	case CID_TIMER_COLOR:
		FillRect(pDIS->hDC, &pDIS->rcItem, hBrushes[tempSettings_.colors.timerColor]);
		break;
	case CID_SELECTED_TIMER_COLOR:
		FillRect(pDIS->hDC, &pDIS->rcItem, hBrushes[tempSettings_.colors.selectedTimerColor]);
		break;
	case CID_LAST_SECONDS_COLOR:
		FillRect(pDIS->hDC, &pDIS->rcItem, hBrushes[tempSettings_.colors.lastSecondsColor]);
		break;
	case CID_BACKGROUND_COLOR:
		FillRect(pDIS->hDC, &pDIS->rcItem, hBrushes[tempSettings_.colors.backgroundColor]);
		break;
	}
}

void SettingsWindow::applyTempHotkey(UINT key) {
	int controlId = GetDlgCtrlID(hActiveControl_); // retrieve control ID

	switch (controlId) {
	case CID_START:
		tempSettings_.startKey = key;
		break;
	case CID_TIMER1:
		tempSettings_.timer1Key = key;
		break;
	case CID_TIMER2:
		tempSettings_.timer2Key = key;
		break;
	}

	SetWindowText(hActiveControl_, keyboardMap_[key]);
	hActiveControl_ = nullptr;
}

void SettingsWindow::applyHotkeySavedKey(HWND hCtrl) {
	int controlId = GetDlgCtrlID(hCtrl); // retrieve control ID

	switch (controlId) {
	case CID_START:
		if (keyboardMap_.count(tempSettings_.startKey)) { // Verify key exists
			SetWindowText(hCtrl, keyboardMap_[tempSettings_.startKey]);
		}
		break;
	case CID_TIMER1:
		if (keyboardMap_.count(tempSettings_.timer1Key)) { // Verify key exists
			SetWindowText(hCtrl, keyboardMap_[tempSettings_.timer1Key]);
		}
		break;
	case CID_TIMER2:
		if (keyboardMap_.count(tempSettings_.timer2Key)) { // Verify key exists
			SetWindowText(hCtrl, keyboardMap_[tempSettings_.timer2Key]);
		}
		break;
	}
}

LRESULT SettingsWindow::handleMessage(UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	try
	{
		switch (wMsg)
		{
		case WM_CREATE:
		{
			initializeWindow();
			return 0;
		}
		case WM_DESTROY:
			hwnd_ = NULL;
			return 0;
		case WM_COMMAND: // Control item clicked
			handleControlCommand(lParam);
			return 0;
		case WM_DRAWITEM: // Color controls
		{
			colorHandles(lParam);
			return TRUE;
		}
		case WM_PAINT:
			displayBitmaps();
			break;
		case WM_LBUTTONDOWN:
			if (hActiveControl_) {
				applyHotkeySavedKey(hActiveControl_);
				hActiveControl_ = nullptr;
			}
			break;
		case WM_KEYDOWN:
			if (hActiveControl_) {
				UINT key = (UINT)wParam;
				applyTempHotkey(key);
			}
			break;
		case WM_SYSKEYDOWN: // Specifically ALT key
			if (hActiveControl_ && (UINT)wParam == VK_MENU) {
				UINT key = (UINT)wParam;
				applyTempHotkey(key);
			}
			break;
		}
	}
	catch (const std::exception e)
	{
		exitApp();
	}
	return DefWindowProc(window(), wMsg, wParam, lParam);
}
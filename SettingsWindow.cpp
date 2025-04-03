#include "SettingsWindow.h"
#include "ResourceUtils.h"
#include "CommCtrlUtils.h"
#include "SettingsUtils.h"
#include "BaseWindow.h"
#include "Program.h"
#include <CommCtrl.h>
#include <windowsx.h>
#include <exception>


void SettingsWindow::initializeWindow()
{
	// retrieve settings
	tempSettings_ = getSafeSettingsStruct();

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

void SettingsWindow::displayBitmaps() const
{
	// Preset values
	constexpr int mouseX = SIZE_SETTINGS_WIDTH / 2 + 45;
	constexpr int mouseY = 35;
	constexpr int controllerX = mouseX + 75;
	constexpr int controllerY = 35;
	constexpr int bitmapSize = 25;
	constexpr int originalBitmapSize = 400;
	constexpr COLORREF ignoreColor = RGB(100, 100, 100);

	PAINTSTRUCT ps;
	const HDC hdc = BeginPaint(hwnd_, &ps);

	// Create a compatible device context
	const HDC hMemDc = CreateCompatibleDC(hdc);

	// Select the mouse bitmap into the memory DC
	HGDIOBJ oldBitmap = SelectObject(hMemDc, mouseBitmap_);

	// Scale bitmaps
	TransparentBlt(hdc,
	mouseX, mouseY,			 // Target x, y coordinates
		bitmapSize,				         // Target width
		bitmapSize,						 // Target height
		hMemDc,
		0, 0,			         // Source x, y coordinates
		originalBitmapSize,				 // Source width
		originalBitmapSize,				 // Source height
		ignoreColor);

	// Select the controller bitmap into the memory DC
	oldBitmap = SelectObject(hMemDc, controllerBitmap_);

	// Scale bitmaps
	TransparentBlt(hdc,
		controllerX, controllerY,			// Target x, y coordinates
		bitmapSize,									// Target width
		bitmapSize,								   	// Target height
		hMemDc,
		0, 0,							     // Source x, y coordinates
		originalBitmapSize,							 // Source width
		originalBitmapSize,							 // Source height
		ignoreColor);

	// Cleanup
	SelectObject(hMemDc, oldBitmap);
	DeleteDC(hMemDc);

	EndPaint(hwnd_, &ps);
}

void SettingsWindow::initializeTextControls() const
{
	// preset values
	constexpr int xTitle = 15;
	constexpr int yOffset = 35;

	// Initialize headers
	const HWND hwndTitleHotkeys = createControl(WC_STATIC, L"Hotkeys", SIZE_SETTINGS_WIDTH / 2 - 40, 5, 60, 40);
	const HWND hwndTitleOptions = createControl(WC_STATIC, L"Options", SIZE_SETTINGS_WIDTH / 2 - 40, yOffset * 6 - 20, 80, 40);
	const HWND hwndTitleColors = createControl(WC_STATIC, L"Colors", SIZE_SETTINGS_WIDTH / 2 - 40, yOffset * 11 - 30, 60, 40);

	// Hotkey titles
	const HWND hwndTextStart = createControl(WC_STATIC, L"Start / Stop / Reset", xTitle, yOffset * 2, 150, 40);
	const HWND hwndTextTimer1 = createControl(WC_STATIC, L"Timer 1", xTitle, yOffset * 3, 150, 20);
	const HWND hwndTextTimer2 = createControl(WC_STATIC, L"Timer 2", xTitle, yOffset * 4, 150, 20);

	// Checkbox titles
	const HWND hwndTextStartOnChange = createControl(WC_STATIC, L"Start On Change", xTitle, yOffset * 7 - 20, 150, 20);
	const HWND hwndTextTransparentBackground = createControl(WC_STATIC, L"Transparent Background", xTitle, yOffset * 8 - 20, 150, 20);
	const HWND hwndTextCheckboxClickthrough = createControl(WC_STATIC, L"Clickthrough (resets when app is closed)", xTitle, yOffset * 9 - 20, 150, 40);

	// Break lines
	const HWND hwndBreakLine1 = createControl(WC_STATIC, nullptr, 15, yOffset * 6, SIZE_SETTINGS_WIDTH - 40, 5, NULL, SS_ETCHEDHORZ);
	const HWND hwndBreakLine2 = createControl(WC_STATIC, nullptr, 15, yOffset * 11 - 10, SIZE_SETTINGS_WIDTH - 40, 5, NULL, SS_ETCHEDHORZ);

	// Color options names
	const HWND hwndTextColorTimer = createControl(WC_STATIC, L"Timer", xTitle, yOffset * 12 - 20, 150, 40);
	const HWND hwndTextColorSelectedTimer = createControl(WC_STATIC, L"Selected Timer", xTitle, yOffset * 13 - 20, 150, 40);
	const HWND hwndTextColorWinCon = createControl(WC_STATIC, L"Last 20 Seconds", xTitle, yOffset * 14 - 20, 150, 40);
	const HWND hwndTextColorBackground = createControl(WC_STATIC, L"Background", xTitle, yOffset * 15 - 20, 150, 40);

	// Copyright text
	const HWND hwndCopyright = createControl(WC_STATIC, L"© Truueh 2025", 10, SIZE_SETTINGS_HEIGHT - 65, 100, 40);

	// Apply fonts
	setControlsFont(hwnd_);
	setTitleFont(hwndTitleColors);
	setTitleFont(hwndTitleHotkeys);
	setCopyrightFont(hwndCopyright);
}

void SettingsWindow::initializeButtonControls()
{
	// preset values
	constexpr int yOffset = 35;

	constexpr int xHotkey = SIZE_SETTINGS_WIDTH / 2 + 20;
	constexpr int xHotkeyCon = xHotkey + 80;
	constexpr int xColorButton = xHotkeyCon - 50;
	constexpr int widthHotkey = 70;
	constexpr int heightHotkey = 20;
	constexpr int sizeCheckbox = 40;
	constexpr int xCheckbox = SIZE_SETTINGS_WIDTH - 70;

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
	const HWND hCbStartOnChange = createControl(WC_BUTTON, L"", xCheckbox, yOffset * 7 - 30, sizeCheckbox, sizeCheckbox, CID_STARTONCHANGE_CB, BS_CHECKBOX | BS_AUTOCHECKBOX);
	const HWND hCbTransparentBg = createControl(WC_BUTTON, L"", xCheckbox, yOffset * 8 - 30, sizeCheckbox, sizeCheckbox, CID_TRANSPARENT_CB, BS_CHECKBOX | BS_AUTOCHECKBOX);
	const HWND hCbClickthrough = createControl(WC_BUTTON, L"", xCheckbox, yOffset * 9 - 30, sizeCheckbox, sizeCheckbox, CID_CLICKTHROUGH_CB, BS_CHECKBOX | BS_AUTOCHECKBOX);

	// Color buttons
	colorButtons[0] = createControl(WC_BUTTON, L"", xColorButton, yOffset * 12 - 20, 100, 15, CID_TIMER_COLOR, BS_OWNERDRAW);
	colorButtons[1] = createControl(WC_BUTTON, L"", xColorButton, yOffset * 13 - 20, 100, 15, CID_SELECTED_TIMER_COLOR, BS_OWNERDRAW);
	colorButtons[2] = createControl(WC_BUTTON, L"", xColorButton, yOffset * 14 - 20, 100, 15, CID_LAST_SECONDS_COLOR, BS_OWNERDRAW);
	colorButtons[3] = createControl(WC_BUTTON, L"", xColorButton, yOffset * 15 - 20, 100, 15, CID_BACKGROUND_COLOR, BS_OWNERDRAW);

	// Initialize exit controls
	HWND hwndOkButton = createControl(WC_BUTTON, L"OK", SIZE_SETTINGS_WIDTH - 160, SIZE_SETTINGS_HEIGHT - 80, 50, 25, CID_OK);
	HWND hwndCancelButton = createControl(WC_BUTTON, L"CANCEL", SIZE_SETTINGS_WIDTH - 100, SIZE_SETTINGS_HEIGHT - 80, 70, 25, CID_CANCEL);

	// Apply font
	setControlsFont(hwnd_);
	
	// Apply currently set hotkeys
	for (const HWND hCtrl : hotkeys) {
		applyHotkeySavedKey(hCtrl);
	}

	// Apply currently set options
	SendMessage(hCbStartOnChange, BM_SETCHECK, appSettings.optionStartOnChange, 0);
	SendMessage(hCbTransparentBg, BM_SETCHECK, appSettings.optionTransparent, 0);
	SendMessage(hCbClickthrough, BM_SETCHECK, appSettings.optionClickThrough, 0);
}

HWND SettingsWindow::createControl(
	const LPCWSTR className, const LPCWSTR controlName, 
	const int x, const int y, const int width, const int height, 
	const int id,const long additionalStyle
) const
{
	const HWND ctrlHwnd = CreateWindowEx(
		0, className, controlName, 
		WS_VISIBLE | WS_CHILDWINDOW | additionalStyle, x,
		y, width, height, hwnd_, (HMENU)id, nullptr, nullptr);

	// Add custom control window proc
	SetWindowSubclass(ctrlHwnd, ctrlWndProc, 1, 0);

	return ctrlHwnd;
}

void SettingsWindow::setCopyrightFont(const HWND hControl)
{
	HFONT hFont = CreateFont(
		14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial"
	);

	SendMessage(hControl, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void SettingsWindow::handleControlCommand(const LPARAM lParam)
{
	const HWND hwndCtrl = reinterpret_cast<HWND>(lParam); // clicked item handle
	const int controlId = GetDlgCtrlID(hwndCtrl); // retrieve control ID

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

	// Start on change checkbox
	case CID_STARTONCHANGE_CB:
		tempSettings_.optionStartOnChange = (Button_GetCheck(hwndCtrl) == BST_CHECKED);
		break;

	// Transparent background checkbox
	case CID_TRANSPARENT_CB: 
		tempSettings_.optionTransparent = (Button_GetCheck(hwndCtrl) == BST_CHECKED);
		break;

	// Clickthrough checkbox
	case CID_CLICKTHROUGH_CB:
		tempSettings_.optionClickThrough = (Button_GetCheck(hwndCtrl) == BST_CHECKED);
		break;

	// Any color control clicked
	case CID_TIMER_COLOR:
	case CID_SELECTED_TIMER_COLOR:
	case CID_LAST_SECONDS_COLOR:
	case CID_BACKGROUND_COLOR:
	{
		// Open a Color Picker window
		if (pColorPicker->window() == nullptr)
		{
			pColorPicker->controlId = controlId; // Notify Color Picker who called it
			pColorPicker->pTempSettings = &tempSettings_;
		
			if (!pColorPicker->create(L"Color Picker", 850, 300, SIZE_COLORPICKER_WIDTH, SIZE_COLORPICKER_HEIGHT, 0, WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX, hwnd_, nullptr, nullptr)) {
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
	default:
		break;
	}
}

void SettingsWindow::colorHandles(const LPARAM lParam) const
{
	const LPDRAWITEMSTRUCT pDis = (LPDRAWITEMSTRUCT)lParam;
	// In case of invalid color index (to prevent index out of range)
	if (tempSettings_.colors.timerColor > 24) {
		FillRect(pDis->hDC, &pDis->rcItem, hBrushes[0]);
		return;
	}

	// Apply saved color to the color controls in the setting's window
	switch (pDis->CtlID)
	{
	case CID_TIMER_COLOR:
		FillRect(pDis->hDC, &pDis->rcItem, hBrushes[tempSettings_.colors.timerColor]);
		break;
	case CID_SELECTED_TIMER_COLOR:
		FillRect(pDis->hDC, &pDis->rcItem, hBrushes[tempSettings_.colors.selectedTimerColor]);
		break;
	case CID_LAST_SECONDS_COLOR:
		FillRect(pDis->hDC, &pDis->rcItem, hBrushes[tempSettings_.colors.lastSecondsColor]);
		break;
	case CID_BACKGROUND_COLOR:
		FillRect(pDis->hDC, &pDis->rcItem, hBrushes[tempSettings_.colors.backgroundColor]);
		break;
	default:
		break;
	}
}

void SettingsWindow::applyTempHotkey(const UINT key) {
	const int controlId = GetDlgCtrlID(hActiveControl_); // retrieve control ID

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
	default:
		break;
	}

	SetWindowText(hActiveControl_, keyboardMap_[key]);
	hActiveControl_ = nullptr;
}

void SettingsWindow::applyHotkeySavedKey(const HWND hCtrl) {
	const int controlId = GetDlgCtrlID(hCtrl); // retrieve control ID

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
	default:
		break;
	}
}

LRESULT SettingsWindow::handleMessage(const UINT wMsg, const WPARAM wParam, const LPARAM lParam)
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
			hwnd_ = nullptr;
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
		case WM_XBUTTONDOWN:
			if (hActiveControl_)
			{
				const UINT key = HIWORD(wParam) == 1 ? VK_XBUTTON1 : VK_XBUTTON2;
				applyTempHotkey(key);
			}
			break;
		case WM_MBUTTONDOWN:
			if (hActiveControl_)
			{
				applyTempHotkey(VK_MBUTTON);
			}
			break;
		case WM_RBUTTONDOWN:
			if (hActiveControl_)
			{
				applyTempHotkey(VK_RBUTTON);
			}
			break;
		case WM_LBUTTONDOWN:
			if (hActiveControl_) {
				applyTempHotkey(VK_LBUTTON);
			}
			break;
		case WM_KEYDOWN:
			if (hActiveControl_) {
				const UINT key = (UINT)wParam;
				applyTempHotkey(key);
			}
			break;
		case WM_SYSKEYDOWN: // Specifically ALT key
			if (hActiveControl_ && (UINT)wParam == VK_MENU) {
				const UINT key = (UINT)wParam;
				applyTempHotkey(key);
			}
			break;
		default:
			break;
		}
	}
	catch (const std::exception &e)
	{
		exitApp();
	}
	return DefWindowProc(window(), wMsg, wParam, lParam);
}

LRESULT CALLBACK SettingsWindow::ctrlWndProc(
	const HWND hwnd, 
	const UINT uMsg, 
	const WPARAM wParam, 
	const LPARAM lParam, 
	const UINT_PTR uIdSubclass,
	const DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
		{
			const HWND settingsHwnd = pGlobalTimerWindow->pSettingsWindow->window();
			const bool isCtrlActive = pGlobalTimerWindow->pSettingsWindow->hActiveControl_ != nullptr;
			if (settingsHwnd != nullptr && isCtrlActive)
			{
				SendMessage(settingsHwnd, uMsg, wParam, lParam);
				return 0;
			}
		}
		break;
	case WM_LBUTTONDBLCLK: // Redirect double clicks into normal clicks
		SendMessage(hwnd, WM_LBUTTONDOWN, wParam, lParam);
		return 0;
	default: 
		break;
	}

	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}
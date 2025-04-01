#include <Windows.h>
#include <CommCtrl.h>
#include "Globals.h"
#include "HelperFunctions.h"
#include "ColorPickerWindow.h"
#include "Program.h"
#include <exception>

// Methods
// Create and store the squared controls that represent selectable colors
void ColorPickerWindow::initializeColorButtons()
{
	int i = 0;
	int baseRowPos = 40;
	int baseColPos = 20;
	int buttonSize = 30;
	int offset = 35;

	for (size_t row = 0; row < 5; row++)
	{

		for (size_t col = 0; col < 5; col++)
		{

			hColorButtons_[i] = CreateWindowEx(0, WC_BUTTON, L"", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, baseRowPos + offset * col, baseColPos + offset * row, buttonSize, buttonSize, hwnd_, (HMENU)i, NULL, NULL);

			i++;
		}
	}

	// Preview Color
	hPreviewColorButton_ = CreateWindowEx(0, WC_BUTTON, L"", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 105, 210, 40, 40, hwnd_, (HMENU)CID_COLOR_PREVIEW, NULL, NULL);
	switch (controlID)
	{
	case CID_TIMER_COLOR:
		hPreviewColor_ = hBrushes[pTempSettings->colors.timerColor];
		previewColorIndex_ = pTempSettings->colors.timerColor;
		break;
	case CID_SELECTED_TIMER_COLOR:
		hPreviewColor_ = hBrushes[pTempSettings->colors.selectedTimerColor];
		previewColorIndex_ = pTempSettings->colors.selectedTimerColor;
		break;
	case CID_LAST_SECONDS_COLOR:
		hPreviewColor_ = hBrushes[pTempSettings->colors.lastSecondsColor];
		previewColorIndex_ = pTempSettings->colors.lastSecondsColor;
		break;
	case CID_BACKGROUND_COLOR:
		hPreviewColor_ = hBrushes[pTempSettings->colors.backgroundColor];
		previewColorIndex_ = pTempSettings->colors.backgroundColor;
		break;
	}
}

void ColorPickerWindow::initializeWindow()
{
	initializeColorButtons();

	// Initialize exit controls
	HWND hwndOKButton = CreateWindowEx(0, WC_BUTTON, L"OK", WS_VISIBLE | WS_CHILDWINDOW, SIZE_COLORPICKER_WIDTH - 150, SIZE_COLORPICKER_HEIGHT - 70, 50, 25, hwnd_, (HMENU)CID_OK, NULL, NULL);
	HWND hwndCancelButton = CreateWindowEx(0, WC_BUTTON, L"CANCEL", WS_VISIBLE | WS_CHILDWINDOW, SIZE_COLORPICKER_WIDTH - 90, SIZE_COLORPICKER_HEIGHT - 70, 70, 25, hwnd_, (HMENU)CID_CANCEL, NULL, NULL);

	setControlsFont(hwnd_);
}

void ColorPickerWindow::updateSettings()
{
	switch (controlID)
	{
	case CID_TIMER_COLOR:
		pTempSettings->colors.timerColor = previewColorIndex_;
		break;
	case CID_SELECTED_TIMER_COLOR:
		pTempSettings->colors.selectedTimerColor = previewColorIndex_;
		break;
	case CID_LAST_SECONDS_COLOR:
		pTempSettings->colors.lastSecondsColor = previewColorIndex_;
		break;
	case CID_BACKGROUND_COLOR:
		pTempSettings->colors.backgroundColor = previewColorIndex_;
		break;
	}

	RedrawWindow(GetWindow(hwnd_, GW_OWNER), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void ColorPickerWindow::handleControlCommand(LPARAM lParam)
{
	// retrieve clicked control information
	HWND hwndCtrl = reinterpret_cast<HWND>(lParam); // clicked item handle
	int CID = GetDlgCtrlID(hwndCtrl); // retrieve control ID

	// clicked a color
	if (CID >= 0 && CID < 25)
	{
		hPreviewColor_ = hBrushes[CID];
		previewColorIndex_ = CID;
		RedrawWindow(hwnd_, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		return;
	}

	// clicked some other control
	switch (CID)
	{
	case CID_OK:
		updateSettings();
		DestroyWindow(hwnd_);
		break;
	case CID_CANCEL:
		DestroyWindow(hwnd_);
		break;
	}
}

LRESULT ColorPickerWindow::handleMessage(UINT wMsg, WPARAM wParam, LPARAM lParam)
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
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
			if (pDIS->CtlID == CID_COLOR_PREVIEW)
			{
				FillRect(pDIS->hDC, &pDIS->rcItem, hPreviewColor_);
			}
			else
			{
				FillRect(pDIS->hDC, &pDIS->rcItem, hBrushes[pDIS->CtlID]);
			}

			return 0;
		}
		}
	}
	catch (const std::exception e)
	{
		exitApp();
	}
	return DefWindowProc(window(), wMsg, wParam, lParam);
}
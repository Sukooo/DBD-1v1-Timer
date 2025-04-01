#include "MainWindow.h"
#include <windowsx.h>
#include "Globals.h"
#include "HelperFunctions.h"
#include "Program.h"

// Constructor
MainWindow::MainWindow() = default;

// Destructor
MainWindow::~MainWindow() {
	discardGraphicsResources();
}

// Method defenitions
HRESULT MainWindow::createGraphicsResources()
{
	HRESULT hr = S_OK;

	// Set up render target and brush
	if (pRenderTarget_ == nullptr)
	{
		RECT rc;
		GetClientRect(hwnd_, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		D2D1_RENDER_TARGET_PROPERTIES rtProperties = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			96.0f, 96.0f,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		);

		hr = pFactory_->CreateHwndRenderTarget(
			rtProperties,
			D2D1::HwndRenderTargetProperties(hwnd_, size),
			&pRenderTarget_
		);

		if (SUCCEEDED(hr)) {
			// Extract ColorF from HBRUSH
			SettingsStruct settings = getSafeSettingsStruct();

			D2D1_COLOR_F timerColor = hBrushToColorf(hBrushes[settings.colors.timerColor]);
			D2D1_COLOR_F selectedTimerColor = hBrushToColorf(hBrushes[settings.colors.selectedTimerColor]);
			D2D1_COLOR_F lastSecondsColor = hBrushToColorf(hBrushes[settings.colors.lastSecondsColor]);
			backgroundColor_ = hBrushToColorf(hBrushes[settings.colors.backgroundColor_]);

			// timer color brush
			hr = pRenderTarget_->CreateSolidColorBrush(timerColor, &pBrushTimer_);

			// selected timer color brush
			hr = pRenderTarget_->CreateSolidColorBrush(selectedTimerColor, &pBrushSelectedTimer_);

			// last seconds color brush
			hr = pRenderTarget_->CreateSolidColorBrush(lastSecondsColor, &pBrushLastSeconds_);
		}
	}

	return hr;
}

HRESULT MainWindow::createDeviceIndependentResources()
{
	HRESULT hr = S_OK;
	// Set up write factory
	if (pWriteFactory_ == NULL)
	{
		if (pWriteFactory_ == nullptr) {
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pWriteFactory_)
				, reinterpret_cast<IUnknown**>(&pWriteFactory_));
		}

		if (SUCCEEDED(hr) && pTextFormat_ == nullptr)
		{
			// Set up text format
			static const WCHAR fontName[] = L"Sitka";
			static const int fontSize = 34;

			hr = pWriteFactory_->CreateTextFormat(
				fontName,
				NULL,
				DWRITE_FONT_WEIGHT_BOLD,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_EXTRA_EXPANDED,
				fontSize,
				L"",
				&pTextFormat_
			);

			if (SUCCEEDED(hr))
			{
				// Center the text horizontally and vertically.
				pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
			}
		}
	}

	return hr;
}

HRESULT MainWindow::changeFontSize(int fontSize)
{
	safeRelease(&pTextFormat_);

	static const WCHAR fontName[] = L"Sitka";

	HRESULT hr = pWriteFactory_->CreateTextFormat(
		fontName,
		NULL,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_EXTRA_EXPANDED,
		fontSize,
		L"",
		&pTextFormat_
	);

	if (SUCCEEDED(hr))
	{
		// Center the text horizontally and vertically.
		pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
	}

	return hr;
}

int MainWindow::getLargestFontsizeFit()
{
	IDWriteTextFormat* pTempTextFormat;
	IDWriteTextLayout* pTempTextLayout;

	int minSize = 10; int maxSize = 100;
	WCHAR fontFamily[] = L"Sitka";
	bool conditionMet = false;

	wchar_t text[] = L"";

	// Iterate through options untill the largest possible font size is found
	while (!conditionMet)
	{
		HRESULT hr = pWriteFactory_->CreateTextFormat(
			fontFamily,
			nullptr,
			pTextFormat_->GetFontWeight(),
			pTextFormat_->GetFontStyle(),
			pTextFormat_->GetFontStretch(),
			maxSize,
			L"",
			&pTempTextFormat
		);

		if (FAILED(hr)) {
			KillProgram();
		}

		hr = pWriteFactory_->CreateTextLayout(
			text,
			(UINT32)wcslen(text),
			pTempTextFormat,
			winSize_[0],
			winSize_[1],
			&pTempTextLayout
		);

		if (FAILED(hr)) {
			KillProgram();
		}

		// retrieve text size
		DWRITE_TEXT_METRICS layoutMetrics;
		hr = pTempTextLayout->GetMetrics(&layoutMetrics);

		if (FAILED(hr)) {
			KillProgram();
		}

		if (layoutMetrics.width <= winSize_[0] &&
			layoutMetrics.height <= winSize_[1])
		{
			conditionMet = true;
		}
		else
		{
			maxSize--;
		}

		safeRelease(&pTempTextFormat);
		safeRelease(&pTempTextLayout);
	}

	return maxSize;
}

void MainWindow::discardGraphicsResources()
{
	safeRelease(&pFactory_);
	safeRelease(&pRenderTarget_);
	safeRelease(&pBrushTimer_);
	safeRelease(&pBrushSelectedTimer_);
	safeRelease(&pBrushLastSeconds_);
	safeRelease(&pWriteFactory_);
	safeRelease(&pTextFormat_);
}

void MainWindow::adjustRendertargetSize()
{
	D2D1_SIZE_U newSize = D2D1::SizeU(winSize_[0], winSize_[1]);
	pRenderTarget_->Resize(newSize);
}

MousePos MainWindow::getMouseDir(LPARAM lParam, RECT windowPos) {
	int currPos[2] = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	int width = windowPos.right - windowPos.left;
	int height = windowPos.bottom - windowPos.top;

	if (currPos[0] <= spaceOffset_) // left
	{
		if (currPos[1] <= spaceOffset_) { // top left
			return MousePos::topLeft;
		}
		else if (currPos[1] >= height - spaceOffset_) { // bottom left
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
			return MousePos::bottomLeft;
		}
		else { // left
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			return MousePos::left;
		}
	}
	else if (currPos[0] >= width - spaceOffset_) { // right
		if (currPos[1] <= spaceOffset_) { // top right
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
			return MousePos::topRight;
		}
		else if (currPos[1] >= height - spaceOffset_) { // bottom right
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
			return MousePos::bottomRight;
		}
		else { // right
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			return MousePos::right;
		}
	}
	else if (currPos[1] >= height - spaceOffset_) { // bottom
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		return MousePos::bottom;
	}
	else if (currPos[1] <= spaceOffset_) // top
	{
		return MousePos::top;
	}

	return MousePos::none;
}

void MainWindow::handlePainting()
{
	HRESULT hr;
	PAINTSTRUCT ps;
	BeginPaint(hwnd_, &ps);
	pRenderTarget_->BeginDraw();

	// workaround to visible edges issue while transparent
	if (appSettings.optionTransparent) {
		pRenderTarget_->Clear(D2D1::ColorF(0, 0, 0));
	}
	else {
		pRenderTarget_->Clear(backgroundColor_);
	}

	D2D1_RECT_F rect1 = D2D1::RectF(0, 0, winSize_[0] / 2, winSize_[1]);
	D2D1_RECT_F rect2 = D2D1::RectF(winSize_[0] / 2, 0, winSize_[0], winSize_[1]);

	if (pWriteFactory_ != nullptr)
	{
		if (activeTimer_ != NULL)
		{
			// Select color for timer 2
			ID2D1SolidColorBrush* pBrushTimer_2;
			if (timer1.getTimeInMillis() > 0
				&& timer1.getTimeInMillis() - timer2.getTimeInMillis() <= 20000
				&& (timer2.getTimerState() == TimerState::running || timer2.getTimerState() == TimerState::paused)
				&& timer1.getTimeInMillis() - timer2.getTimeInMillis() > 0)
			{
				pBrushTimer_2 = pBrushLastSeconds_;
			}
			else if (activeTimer_ == &timer2) {
				pBrushTimer_2 = pBrushSelectedTimer_;
			}
			else {
				pBrushTimer_2 = pBrushTimer_;
			}

			// draw timers
			if (activeTimer_ == &timer1) {
				timer1.draw(pRenderTarget_, pTextFormat_, rect1, pBrushSelectedTimer_);
			}
			else {
				timer1.draw(pRenderTarget_, pTextFormat_, rect1, pBrushTimer_);
			}
			timer2.draw(pRenderTarget_, pTextFormat_, rect2, pBrushTimer_2);
		}
		else
		{
			timer1.draw(pRenderTarget_, pTextFormat_, rect1, pBrushTimer_);
			timer2.draw(pRenderTarget_, pTextFormat_, rect2, pBrushTimer_);
		}
	}

	hr = pRenderTarget_->EndDraw();

	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
	{
		discardGraphicsResources();
	}
	EndPaint(hwnd_, &ps);
}

void MainWindow::handleMouseMovement(LPARAM lParam) {
	// variables
	RECT windowPos;
	GetWindowRect(hwnd_, &windowPos);
	int currPos[2] = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	int width = windowPos.right - windowPos.left;
	int height = windowPos.bottom - windowPos.top;

	switch (getMouseDir(lParam, windowPos))
	{
	case MousePos::topLeft:
		SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		break;
	case MousePos::top:
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		break;
	case MousePos::topRight:
		SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		break;
	case MousePos::right:
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		break;
	case MousePos::bottomRight:
		SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		break;
	case MousePos::bottom:
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		break;
	case MousePos::bottomLeft:
		SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		break;
	case MousePos::left:
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		break;
	case MousePos::none:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	}

	if (dir_ == -1) // if not hovering resize area:
	{
		if (mouseDown_ && !isResizing_) // dragging the window
		{
			// drag window
			int xToMove = windowPos.left + (currPos[0] - clickMousePos_[0]);
			int yToMove = windowPos.top + (currPos[1] - clickMousePos_[1]);

			SetWindowPos(hwnd_, NULL, xToMove, yToMove, winSize_[0], winSize_[1], 0);
		}
	}

	if (mouseDown_ && isResizing_) // if isResizing_
	{
		int newWidth = winSize_[0]; int newHeight = winSize_[1];
		int newX = windowPos.left; int newY = windowPos.top;

		// isResizing_ logic
		switch (dir_)
		{
		case 0: // horizontal
			if (currPos[1] <= spaceOffset_) // resetting size from the top side
			{
				newY = windowPos.top + (currPos[1] - clickMousePos_[1]);
				newHeight = windowPos.bottom - windowPos.top - (currPos[1] - clickMousePos_[1]);
			}
			else // resetting size from the bottom side
			{
				newHeight = winSize_[1] + currPos[1] - clickMousePos_[1];
			}
			break;
		case 1: // vertical
		{
			if (currPos[0] <= spaceOffset_) // resetting size from the left side
			{
				newX = windowPos.left + (currPos[0] - clickMousePos_[0]);
				newWidth = (windowPos.right - windowPos.left) - (currPos[0] - clickMousePos_[0]);
			}
			else // resetting size from the right side
			{
				newWidth = winSize_[0] + currPos[0] - clickMousePos_[0];
			}

		}
		break;
		case 2:
			if (currPos[0] <= spaceOffset_) // resetting size from the left side
			{
				newX = windowPos.left + (currPos[0] - clickMousePos_[0]);
				newWidth = (windowPos.right - windowPos.left) - (currPos[0] - clickMousePos_[0]);
			}
			else // resetting size from the right side
			{
				newWidth = winSize_[0] + currPos[0] - clickMousePos_[0];
			}
			if (currPos[1] <= spaceOffset_) // resetting size from the top side
			{
				newY = windowPos.top + (currPos[1] - clickMousePos_[1]);
				newHeight = (windowPos.bottom - windowPos.top) - (currPos[1] - clickMousePos_[1]);
			}
			else // resetting size from the bottom side
			{
				newHeight = winSize_[1] + currPos[1] - clickMousePos_[1];
			}
			break;
		}

		if (dir_ != -1) { // window was resized
			newWidth = max(25, min(700, newWidth));
			newHeight = max(25, min(700, newHeight));
			SetWindowPos(hwnd_, NULL, newX, newY, newWidth, newHeight, 0);
		}
	}
}

D2D1_COLOR_F MainWindow::hBrushToColorf(HBRUSH hBrush) {
	LOGBRUSH logBrush;

	// timer color
	GetObject(hBrush, sizeof(LOGBRUSH), &logBrush);
	COLORREF color = logBrush.lbColor; // Extract COLORREF
	return D2D1::ColorF(GetRValue(color) / 255.0f, GetGValue(color) / 255.0f, GetBValue(color) / 255.0f);
}

void MainWindow::refreshBrushes()
{
	// retrieve brushes colors
	SettingsStruct settings = getSafeSettingsStruct();

	LOGBRUSH logBrush;

	// timer color
	pBrushTimer_->SetColor(hBrushToColorf(hBrushes[settings.colors.timerColor]));

	// selected timer color
	pBrushSelectedTimer_->SetColor(hBrushToColorf(hBrushes[settings.colors.selectedTimerColor]));

	// last seconds color
	pBrushLastSeconds_->SetColor(hBrushToColorf(hBrushes[settings.colors.lastSecondsColor]));

	// background color
	backgroundColor_ = hBrushToColorf(hBrushes[settings.colors.backgroundColor_]);
}

LRESULT MainWindow::handleMessage(UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	try
	{
		RECT windowPos;
		GetWindowRect(hwnd_, &windowPos);

		switch (wMsg)
		{
		case WM_CREATE:
		{
			if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pFactory_))) {
				return -1;
			}
			else {
				createGraphicsResources();
				createDeviceIndependentResources();
			}
			appSettings = getSafeSettingsStruct();
			appRunning = true;
			return 0;
		}
		case WM_DESTROY:
		{
			KillProgram();
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			mouseDown_ = true;
			clickMousePos_[0] = GET_X_LPARAM(lParam); clickMousePos_[1] = GET_Y_LPARAM(lParam);
			isResizing_ = (GetCursor() != LoadCursor(NULL, IDC_ARROW));
			MousePos mPos = getMouseDir(lParam, windowPos);

			// set drag direction according to mouse position
			if (mPos == MousePos::topRight || mPos == MousePos::topLeft ||
				mPos == MousePos::bottomLeft || mPos == MousePos::bottomRight)
			{
				dir_ = 2;
			}
			else if (mPos == MousePos::right || mPos == MousePos::left)
			{
				dir_ = 1;
			}
			else if (mPos == MousePos::top || mPos == MousePos::bottom)
			{
				dir_ = 0;
			}
			else
			{
				dir_ = -1;
			}

			SetCapture(hwnd_);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			mouseDown_ = false;
			isResizing_ = false;
			dir_ = -1;
			ReleaseCapture();

			// update winSize_ var after isResizing_
			if (windowPos.right - windowPos.left != winSize_[0]) {
				winSize_[0] = windowPos.right - windowPos.left;
				adjustRendertargetSize();
				changeFontSize(getLargestFontsizeFit());
			}
			if (windowPos.bottom - windowPos.top != winSize_[1]) {
				winSize_[1] = windowPos.bottom - windowPos.top;
				adjustRendertargetSize();
				changeFontSize(getLargestFontsizeFit());
			}
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			handleMouseMovement(lParam);
			return 0;
		}
		case WM_COMMAND:
		{
			switch (wParam)
			{
			case MENU_SETTINGS:
				if (pSettingsWindow->window() == NULL) // dont create multiple settings windows
				{
					// Create and show settings window
					if (!pSettingsWindow->create(L"Settings - Version 1.2", 500, 200, SIZE_SETTINGS_WIDTH, SIZE_SETTINGS_HEIGHT, 0, WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX, hwnd_, 0, 0, NULL)) {
						return 0;
					}

					ShowWindow(pSettingsWindow->window(), SW_SHOW);
				}
				else
				{
					SetForegroundWindow(pSettingsWindow->window());
				}
				return 0;
			case MENU_QUIT:
				KillProgram();
				return 0;
			}
			return 0;
		}
		case WM_CONTEXTMENU:
		{
			// mouse pos
			int mouseX = GET_X_LPARAM(lParam);
			int mouseY = GET_Y_LPARAM(lParam);

			// create popup menu
			HMENU hMenu = CreatePopupMenu();
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, MENU_QUIT, L"Quit");
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 100, L"");
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, MENU_SETTINGS, L"Settings");
			SetForegroundWindow(hwnd_);
			TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN, mouseX, mouseY, 0, hwnd_, NULL);
			return 0;
		}
		case WM_SETCURSOR:
			// disable default automatic cursor change (only manually set it)
			return 0;
		case REFRESH_BRUSHES:
			refreshBrushes();
			break;
		}
	}
	catch (const std::exception e) // End the program in case of exceptions
	{
		KillProgram();
	}

	return DefWindowProc(window(), wMsg, wParam, lParam);
}

void MainWindow::handleHotKey(int code)
{
	switch (code)
	{
	case KEY_TIMER1: // timer1 key
		activeTimer_ = &timer1;
		break;
	case KEY_TIMER2: // timer2 key
		activeTimer_ = &timer2;
		break;
	case KEY_START: // start key
		if (activeTimer_ != NULL)
		{
			if (activeTimer_->getTimerState() == TimerState::zero) {
				activeTimer_->startTimer();
			}
			else if (activeTimer_->getTimerState() == TimerState::running) {
				activeTimer_->stopTimer();
			}
			else {
				activeTimer_->resetTimer();
			}
		}
		break;
	}
}

void MainWindow::draw() {
	handlePainting();
}
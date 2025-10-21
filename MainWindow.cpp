#include "MainWindow.h"
#include <windowsx.h>
#include "Globals.h"
#include "HotkeyManager.h"
#include "ResourceUtils.h"
#include "SettingsUtils.h"
#include "Program.h"
#include "Configuration.h"
#include "RenderManager.h"
#include "SettingsManager.h"
#include "WindowStateManager.h"


MainWindow::MainWindow() {
	// Set up timer for periodic updates (100ms interval = 10 Hz)
	// Will be initialized after window is created
}

MainWindow::~MainWindow() {
	// Kill timer
	if (hwnd_) {
		KillTimer(hwnd_, TIMER_UPDATE_ID);
	}
	
	// Unsubscribe from events
	EventManager::getInstance().unsubscribe(EventType::SettingsChanged, this);
	
	discardGraphicsResources();
}

HRESULT MainWindow::createGraphicsResources()
{
	HRESULT hr = S_OK;

	// Set up render target and brush
	if (pRenderTarget_ == nullptr)
	{
		RECT rc;
		GetClientRect(hwnd_, &rc);

		const D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		const D2D1_RENDER_TARGET_PROPERTIES rtProperties = D2D1::RenderTargetProperties(
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
			const SettingsStruct settings = getSafeSettingsStruct();

			const D2D1_COLOR_F timerColor = hBrushToColorf(hBrushes[settings.colors.timerColor]);
			const D2D1_COLOR_F selectedTimerColor = hBrushToColorf(hBrushes[settings.colors.selectedTimerColor]);
			const D2D1_COLOR_F lastSecondsColor = hBrushToColorf(hBrushes[settings.colors.lastSecondsColor]);
			backgroundColor_ = hBrushToColorf(hBrushes[settings.colors.backgroundColor]);

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
	if (pWriteFactory_ == nullptr)
	{
		if (pWriteFactory_ == nullptr) {
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pWriteFactory_)
				, reinterpret_cast<IUnknown**>(&pWriteFactory_));
		}

		if (SUCCEEDED(hr) && pTextFormat_ == nullptr)
		{
			// Set up text format
			static constexpr WCHAR fontName[] = L"Sitka";
			static constexpr int fontSize = 34;

			hr = pWriteFactory_->CreateTextFormat(
				fontName,
				nullptr,
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
				hr = pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

				if (SUCCEEDED(hr))
				{
					hr = pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
				}
			}
		}
	}

	return hr;
}

HRESULT MainWindow::changeFontSize(const float fontSize)
{
	safeRelease(&pTextFormat_);

	static constexpr WCHAR fontName[] = L"Sitka";

	HRESULT hr = pWriteFactory_->CreateTextFormat(
		fontName,
		nullptr,
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
		hr = pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		if (SUCCEEDED(hr))
		{
			hr = pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		}
	}

	return hr;
}

float MainWindow::getLargestFontsizeFit() const
{
	IDWriteTextFormat* pTempTextFormat;
	IDWriteTextLayout* pTempTextLayout;

	float maxSize = 100;
	constexpr WCHAR fontFamily[] = L"Sitka";
	bool conditionMet = false;

	constexpr wchar_t text[] = L"";

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
			exitApp();
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
			exitApp();
		}

		// retrieve text size
		DWRITE_TEXT_METRICS layoutMetrics;
		hr = pTempTextLayout->GetMetrics(&layoutMetrics);

		if (FAILED(hr)) {
			exitApp();
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

HRESULT MainWindow::adjustRendertargetSize() const
{
	const D2D1_SIZE_U newSize = D2D1::SizeU(winSize_[0], winSize_[1]);
	return pRenderTarget_->Resize(newSize);
}

MousePos MainWindow::getMouseDir(const LPARAM lParam, const RECT windowPos) const {
	const int currPos[2] = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	const int width = windowPos.right - windowPos.left;
	const int height = windowPos.bottom - windowPos.top;

	if (currPos[0] <= spaceOffset_) // Left
	{
		if (currPos[1] <= spaceOffset_) { // Top Left
			return MousePos::TopLeft;
		}
		else if (currPos[1] >= height - spaceOffset_) { // Bottom Left
			SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
			return MousePos::BottomLeft;
		}
		else { // Left
			SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
			return MousePos::Left;
		}
	}
	else if (currPos[0] >= width - spaceOffset_) { // Right
		if (currPos[1] <= spaceOffset_) { // Top Right
			SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
			return MousePos::TopRight;
		}
		else if (currPos[1] >= height - spaceOffset_) { // Bottom Right
			SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
			return MousePos::BottomRight;
		}
		else { // Right
			SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
			return MousePos::Right;
		}
	}
	else if (currPos[1] >= height - spaceOffset_) { // Bottom
		SetCursor(LoadCursor(nullptr, IDC_SIZENS));
		return MousePos::Bottom;
	}
	else if (currPos[1] <= spaceOffset_) // Top
	{
		return MousePos::Top;
	}

	return MousePos::None;
}

void MainWindow::handlePainting()
{
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

	const D2D1_RECT_F rect1 = D2D1::RectF(0, 0, winSize_[0] / 2, winSize_[1]);
	const D2D1_RECT_F rect2 = D2D1::RectF(winSize_[0] / 2, 0, winSize_[0], winSize_[1]);

	if (pWriteFactory_ != nullptr)
	{
		if (activeTimer_ != nullptr)
		{
			// Select color for timer 2
			ID2D1SolidColorBrush* pBrushTimer_2;
			if (timer1.getTimeInMillis() > 0
				&& timer1.getTimeInMillis() - timer2.getTimeInMillis() <= 20000
				&& (timer2.getTimerState() == TimerState::Running || timer2.getTimerState() == TimerState::Paused)
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

	const HRESULT hr = pRenderTarget_->EndDraw();

	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
	{
		discardGraphicsResources();
	}
	EndPaint(hwnd_, &ps);
}

void MainWindow::handleMouseMovement(const LPARAM lParam) const {
	// variables
	RECT windowPos;
	GetWindowRect(hwnd_, &windowPos);
	const int currPos[2] = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	switch (getMouseDir(lParam, windowPos))
	{
	case MousePos::TopLeft:
		SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
		break;
	case MousePos::Top:
		SetCursor(LoadCursor(nullptr, IDC_SIZENS));
		break;
	case MousePos::TopRight:
		SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
		break;
	case MousePos::Right:
		SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
		break;
	case MousePos::BottomRight:
		SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
		break;
	case MousePos::Bottom:
		SetCursor(LoadCursor(nullptr, IDC_SIZENS));
		break;
	case MousePos::BottomLeft:
		SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
		break;
	case MousePos::Left:
		SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
		break;
	case MousePos::None:
		SetCursor(LoadCursor(nullptr, IDC_ARROW));
		break;
	}

	if (dir_ == -1) // if not hovering resize area:
	{
		if (mouseDown_ && !isResizing_) // dragging the window
		{
			// drag window
			const int xToMove = windowPos.left + (currPos[0] - clickMousePos_[0]);
			const int yToMove = windowPos.top + (currPos[1] - clickMousePos_[1]);

			SetWindowPos(hwnd_, nullptr, xToMove, yToMove, winSize_[0], winSize_[1], 0);
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
			if (currPos[1] <= spaceOffset_) // resetting size from the Top side
			{
				newY = windowPos.top + (currPos[1] - clickMousePos_[1]);
				newHeight = windowPos.bottom - windowPos.top - (currPos[1] - clickMousePos_[1]);
			}
			else // resetting size from the Bottom side
			{
				newHeight = winSize_[1] + currPos[1] - clickMousePos_[1];
			}
			break;
		case 1: // vertical
		{
			if (currPos[0] <= spaceOffset_) // resetting size from the Left side
			{
				newX = windowPos.left + (currPos[0] - clickMousePos_[0]);
				newWidth = (windowPos.right - windowPos.left) - (currPos[0] - clickMousePos_[0]);
			}
			else // resetting size from the Right side
			{
				newWidth = winSize_[0] + currPos[0] - clickMousePos_[0];
			}
		}
		break;
		case 2:
			if (currPos[0] <= spaceOffset_) // resetting size from the Left side
			{
				newX = windowPos.left + (currPos[0] - clickMousePos_[0]);
				newWidth = (windowPos.right - windowPos.left) - (currPos[0] - clickMousePos_[0]);
			}
			else // resetting size from the Right side
			{
				newWidth = winSize_[0] + currPos[0] - clickMousePos_[0];
			}
			if (currPos[1] <= spaceOffset_) // resetting size from the Top side
			{
				newY = windowPos.top + (currPos[1] - clickMousePos_[1]);
				newHeight = (windowPos.bottom - windowPos.top) - (currPos[1] - clickMousePos_[1]);
			}
			else // resetting size from the Bottom side
			{
				newHeight = winSize_[1] + currPos[1] - clickMousePos_[1];
			}
			break;
		default:
			return;
		}

		if (dir_ != -1) { // window was resized
			newWidth = max(25, min(700, newWidth));
			newHeight = max(25, min(700, newHeight));
			SetWindowPos(hwnd_, nullptr, newX, newY, newWidth, newHeight, 0);
		}
	}
}

D2D1_COLOR_F MainWindow::hBrushToColorf(const HBRUSH hBrush) {
	LOGBRUSH logBrush;

	// timer color
	GetObject(hBrush, sizeof(LOGBRUSH), &logBrush);
	const COLORREF color = logBrush.lbColor; // Extract COLORREF
	return D2D1::ColorF(GetRValue(color) / 255.0f, GetGValue(color) / 255.0f, GetBValue(color) / 255.0f);
}

void MainWindow::refreshBrushes()
{
	// retrieve brushes colors
	const SettingsStruct settings = getSafeSettingsStruct();

	LOGBRUSH logBrush;

	// timer color
	pBrushTimer_->SetColor(hBrushToColorf(hBrushes[settings.colors.timerColor]));

	// selected timer color
	pBrushSelectedTimer_->SetColor(hBrushToColorf(hBrushes[settings.colors.selectedTimerColor]));

	// last seconds color
	pBrushLastSeconds_->SetColor(hBrushToColorf(hBrushes[settings.colors.lastSecondsColor]));

	// background color
	backgroundColor_ = hBrushToColorf(hBrushes[settings.colors.backgroundColor]);
}

void MainWindow::onEvent(const Event& event) {
	switch (event.type) {
		case EventType::SettingsChanged:
		{
			refreshBrushes();
			// Apply new opacity
			const auto& settings = SettingsManager::getInstance().getSettings();
			SetLayeredWindowAttributes(hwnd_, 0, settings.opacity, LWA_ALPHA);
			
			// Mark for redraw
			RenderManager::getInstance().markDirty();
		}
		break;
		
		case EventType::ThemeChanged:
			refreshBrushes();
			RenderManager::getInstance().markDirty();
			break;
			
		case EventType::TimerStateChanged:
			RenderManager::getInstance().markDirty();
			break;
			
		default:
			break;
	}
}

LRESULT MainWindow::handleMessage(const UINT wMsg, const WPARAM wParam, const LPARAM lParam)
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
			createGraphicsResources();
			createDeviceIndependentResources();

			// Load settings from SettingsManager
			if (!SettingsManager::getInstance().fileExists()) {
				SettingsManager::getInstance().createDefaultFile();
			}
			SettingsManager::getInstance().loadFromFile();
			appSettings = SettingsManager::getInstance().getSettings();
			
			// Set up timer for periodic updates
			SetTimer(hwnd_, TIMER_UPDATE_ID, Config::RenderingConfig::TIMER_UPDATE_INTERVAL_MS, nullptr);
			
			// Set up render callback
			RenderManager::getInstance().setRenderCallback([this]() {
				this->draw();
			});
			
			// Subscribe to events
			EventManager::getInstance().subscribe(EventType::SettingsChanged, this);
			
			appRunning = true;
			return 0;
		}
		case WM_DESTROY:
		{
			// Save window state before exit
			WindowStateManager::getInstance().saveWindowState(hwnd_);
			exitApp();
			return 0;
		}
		case WM_TIMER:
			if (wParam == TIMER_UPDATE_ID) {
				// Update timers
				timer1.updateTime();
				timer2.updateTime();
				
				// Emit event if state changed (timers handle this internally)
				Event evt{EventType::TimerStateChanged, nullptr};
				EventManager::getInstance().emit(evt);
				
				// Check if render is needed
				if (RenderManager::getInstance().shouldRender()) {
					InvalidateRect(hwnd_, nullptr, FALSE);
				}
			}
			return 0;
			
		case WM_PAINT:
		{
			if (RenderManager::getInstance().isDirty()) {
				handlePainting();
				RenderManager::getInstance().render();  // Clears dirty flag
			}
			ValidateRect(hwnd_, nullptr);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			mouseDown_ = true;
			clickMousePos_[0] = GET_X_LPARAM(lParam); clickMousePos_[1] = GET_Y_LPARAM(lParam);
			isResizing_ = (GetCursor() != LoadCursor(NULL, IDC_ARROW));
			MousePos mPos = getMouseDir(lParam, windowPos);

			// set drag direction according to mouse position
			if (mPos == MousePos::TopRight || mPos == MousePos::TopLeft ||
				mPos == MousePos::BottomLeft || mPos == MousePos::BottomRight)
			{
				dir_ = 2;
			}
			else if (mPos == MousePos::Right || mPos == MousePos::Left)
			{
				dir_ = 1;
			}
			else if (mPos == MousePos::Top || mPos == MousePos::Bottom)
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
			
			if (isResizing_) {
				// Cache font size calculation - only recalculate if size changed
				SIZE currentSize = {windowPos.right - windowPos.left, windowPos.bottom - windowPos.top};
				if (currentSize.cx != cachedWindowSize_.cx || currentSize.cy != cachedWindowSize_.cy) {
					cachedFontSize_ = getLargestFontsizeFit();
					changeFontSize(cachedFontSize_);
					cachedWindowSize_ = currentSize;
				}
				
				isResizing_ = false;
			}
			
			// Update winSize_ var after resize
			if (windowPos.right - windowPos.left != winSize_[0]) {
				winSize_[0] = windowPos.right - windowPos.left;
				adjustRendertargetSize();
			}
			if (windowPos.bottom - windowPos.top != winSize_[1]) {
				winSize_[1] = windowPos.bottom - windowPos.top;
				adjustRendertargetSize();
			}
			
			// Save window state after drag or resize
			WindowStateManager::getInstance().saveWindowState(hwnd_);
			
			// Apply snapping
			WindowStateManager::getInstance().applySnapping(hwnd_);
			
			dir_ = -1;
			ReleaseCapture();
			RenderManager::getInstance().markDirty();
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
				if (pSettingsWindow->window() == nullptr) // dont create multiple settings windows
				{
					// Create and show settings window
					if (!pSettingsWindow->create(L"Settings - Version 1.4", 500, 200, SIZE_SETTINGS_WIDTH, SIZE_SETTINGS_HEIGHT, 0, WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX, hwnd_, nullptr	, nullptr, nullptr)) {
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
				exitApp();
				return 0;
			default:
				break;
			}
			return 0;
		}
		case WM_CONTEXTMENU:
		{
			// mouse pos
			const int mouseX = GET_X_LPARAM(lParam);
			const int mouseY = GET_Y_LPARAM(lParam);

			// create popup menu
			HMENU hMenu = CreatePopupMenu();
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, MENU_QUIT, L"Quit");
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 100, L"");
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, MENU_SETTINGS, L"Settings");
			SetForegroundWindow(hwnd_);
			TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN, mouseX, mouseY, 0, hwnd_, nullptr);
			return 0;
		}
		case WM_SETCURSOR:
			// disable default automatic cursor change (only manually set it)
			return 0;
		case REFRESH_BRUSHES:
			refreshBrushes();
			break;
		case HOTKEY_HIT:
		{
			const int key = (int)wParam;
			handleHotKey(key);
		}
			break;
		case CONTROLLER_INPUT:
			handleControllerInput(wParam);
			break;
		default:
			break;
		}
	}
	catch (const std::exception &e) // End the program in case of exceptions
	{
		exitApp();
	}

	return DefWindowProc(window(), wMsg, wParam, lParam);
}

void MainWindow::handleHotKey(const int code)
{
	const auto& settings = SettingsManager::getInstance().getSettings();
	bool isActivateTimer = settings.optionStartOnChange;

	switch (code)
	{
	case KEY_TIMER1: // timer1 key
		activeTimer_ = &timer1;
		break;
	case KEY_TIMER2: // timer2 key
		activeTimer_ = &timer2;
		break;
	case KEY_START: // start key
		isActivateTimer = true;
		break;
	case KEY_START_NO_RESET: // start no reset key
	{
		if (activeTimer_ == nullptr) return;

		if (activeTimer_->getTimerState() == TimerState::Running)
			activeTimer_->stopTimer();
		else
			activeTimer_->startTimer();
	}
		break;
		
	// NEW: Click-through toggle
	case KEY_CLICKTHROUGH_TOGGLE:
	{
		clickThroughActive_ = !clickThroughActive_;
		
		// Apply window style immediately
		LONG style = GetWindowLong(hwnd_, GWL_EXSTYLE);
		if (clickThroughActive_) {
			style |= WS_EX_TRANSPARENT;
		} else {
			style &= ~WS_EX_TRANSPARENT;
		}
		SetWindowLong(hwnd_, GWL_EXSTYLE, style);
	}
		break;
		
	// NEW: Opacity up
	case KEY_OPACITY_UP:
	{
		auto currentSettings = SettingsManager::getInstance().getSettings();
		currentSettings.opacity = min(currentSettings.opacity + Config::OpacityConfig::OPACITY_STEP,
		                              Config::OpacityConfig::MAX_OPACITY);
		SettingsManager::getInstance().updateSettings(currentSettings);
		SetLayeredWindowAttributes(hwnd_, 0, currentSettings.opacity, LWA_ALPHA);
	}
		break;
		
	// NEW: Opacity down
	case KEY_OPACITY_DOWN:
	{
		auto currentSettings = SettingsManager::getInstance().getSettings();
		currentSettings.opacity = max(currentSettings.opacity - Config::OpacityConfig::OPACITY_STEP,
		                              Config::OpacityConfig::MIN_OPACITY);
		SettingsManager::getInstance().updateSettings(currentSettings);
		SetLayeredWindowAttributes(hwnd_, 0, currentSettings.opacity, LWA_ALPHA);
	}
		break;
		
	default:
		break;
	}

	if (isActivateTimer && activeTimer_ != nullptr)
	{
		if (activeTimer_->getTimerState() == TimerState::Zero) {
			activeTimer_->startTimer();
		}
		else if (activeTimer_->getTimerState() == TimerState::Running) {
			activeTimer_->stopTimer();
		}
		else {
			activeTimer_->resetTimer();
		}
	}
	
	RenderManager::getInstance().markDirty();
}

void MainWindow::handleControllerInput(const WORD buttons) const
{
	if (pSettingsWindow->window() != nullptr)
	{
		SendMessage(pSettingsWindow->window(), CONTROLLER_INPUT, buttons, NULL);
		return;
	}

	HotkeyManager::execute(buttons);
}

void MainWindow::draw() {
	handlePainting();
}
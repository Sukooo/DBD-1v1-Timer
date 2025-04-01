#include <Windows.h>
#include "Timer.h"

using std::wstring;

int Timer::subtractTimes(SYSTEMTIME t1, SYSTEMTIME t2)
{
	int seconds = 0;
	int minutes = 0;
	int millis = 0;

	int t1Millis = (t1.wMinute * 60 * 1000) + (t1.wSecond * 1000) + t1.wMilliseconds;
	int t2Millis = (t2.wMinute * 60 * 1000) + (t2.wSecond * 1000) + t2.wMilliseconds;

	if (t1Millis > t2Millis) {
		t2Millis += (60 * 60 * 1000);
	}

	millis = t2Millis - t1Millis;

	return millis;
}

Timer::Timer() {
	time_ = 0;
	timerState_ = TimerState::zero;
}

TimerState Timer::getTimerState()
{
	return timerState_;
}

wstring Timer::getTimeAsText()
{
	int minutesInt = 0;
	int secondsInt = 0;
	int millisInt = time_;

	secondsInt = millisInt / 1000;
	minutesInt = secondsInt / 60;
	millisInt = millisInt % 1000;
	secondsInt = secondsInt % 60;

	wstring secondsStr = std::to_wstring(secondsInt);
	wstring minutesStr = std::to_wstring(minutesInt);
	wstring millisStr = std::to_wstring(millisInt);

	if (millisStr.size() == 1) {
		millisStr += '0';
	}

	wstring text = L"              ";

	if (minutesInt < 1) {
		if (secondsInt < 10) {
			text[0] = secondsStr[0];
			text[1] = '.';
			text[2] = millisStr[0];
			text[3] = millisStr[1];
		}
		else {
			text[0] = secondsStr[0];
			text[1] = secondsStr[1];
			text[2] = '.';
			text[3] = millisStr[0];
			text[4] = millisStr[1];
		}
	}
	else {
		if (minutesInt < 10) {
			text[0] = minutesStr[0];
			text[1] = ':';

			if (secondsInt < 10) {
				text[2] = '0'; text[3] = secondsStr[0];
			}
			else {
				text[2] = secondsStr[0]; text[3] = secondsStr[1];
			}
			text[4] = '.';
			text[5] = millisStr[0];
		}
		else {
			text[0] = minutesStr[0];
			text[1] = minutesStr[1];
			text[2] = ':';

			if (secondsInt < 10) {
				text[3] = '0'; text[4] = secondsStr[0];
			}
			else {
				text[3] = secondsStr[0]; text[4] = secondsStr[1];
			}
			text[5] = '.';
			text[6] = millisStr[0];
		}
	}

	return text;
}

int Timer::getTimeInMillis()
{
	return time_;
}

void Timer::startTimer()
{
	timerState_ = TimerState::running;
	GetSystemTime(&startTime_);
}

void Timer::stopTimer()
{
	timerState_ = TimerState::paused;
}

void Timer::resetTimer()
{
	timerState_ = TimerState::zero;
	GetLocalTime(&startTime_);
	GetLocalTime(&updatingTime_);
	time_ = subtractTimes(startTime_, updatingTime_);
}

void Timer::updateTime()
{
	if (timerState_ == TimerState::running)
	{
		GetSystemTime(&updatingTime_);
		time_ = subtractTimes(startTime_, updatingTime_);
	}
}

void Timer::draw(ID2D1HwndRenderTarget* pRenderTarget_, IDWriteTextFormat* pTextFormat_, D2D1_RECT_F rectF, ID2D1SolidColorBrush* pBrush) // Only call from within an active render target begin draw scope
{
	if (pRenderTarget_ != NULL)
	{
		wstring timeStr = getTimeAsText();

		const WCHAR* timeText = timeStr.c_str();

		pRenderTarget_->DrawTextW(
			timeText,
			8,
			pTextFormat_,
			rectF,
			pBrush);
	}
}
#pragma once
#include <unordered_map>
#include "AppSettings.h"

// Forward declaration
class MainWindow;

// Hotkey action constants (moved from globals.h)
constexpr byte KEY_START = 0;
constexpr byte KEY_START_NO_RESET = 5;
constexpr byte KEY_TIMER1 = 1;
constexpr byte KEY_TIMER2 = 2;
constexpr byte OPTION_TRANSPARENT = 3;
constexpr byte OPTION_CLICKTHROUGH = 4;
constexpr byte KEY_CLICKTHROUGH_TOGGLE = 6;
constexpr byte KEY_OPACITY_UP = 7;
constexpr byte KEY_OPACITY_DOWN = 8;

class HotkeyManager
{
public:
	static std::unordered_map<int, int> hotkeysMap;

	/**
	@brief Set the target window for hotkey messages
	
	@param window Pointer to the MainWindow to send hotkey messages to
	*/
	static void setTargetWindow(MainWindow* window);

	/**
	@brief Set the contents of the hotkeyMap

	@param settings A reference to the AppSettings to receive hotkey data from
	*/
	static void setHotkeysMap(const AppSettings &settings);

	/**
	@brief Set the contents of the hotkeyMap

	@param startKey A virtual key for the Start/Stop/Reset hotkey

	@param startKey A virtual key for switching to timer 1

	@param startKey A virtual key for switching to timer 2
	*/
	static void setHotkeysMap(int startKey, int startNoResetKey, int timer1Key, int timer2Key, int conStartKey, int conStartNoResetKey, int conTimer1Key, int conTimer2Key);

	/**
	@brief If the keyCode is linked to a hotkey, post a message to MainWindow activate it.

	@param keyCode The virtual key to execute
	*/
	static void execute(int keyCode);

private:
	static MainWindow* targetWindow_;
};

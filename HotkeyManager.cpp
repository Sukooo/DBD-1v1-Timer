#include "Globals.h"
#include "HotkeyManager.h"

#include "Program.h"

std::unordered_map<int, int> HotkeyManager::hotkeysMap;

void HotkeyManager::setHotkeysMap(const SettingsStruct& settings)
{
	setHotkeysMap(
		settings.startKey, settings.startNoResetKey, settings.timer1Key, settings.timer2Key,
		settings.conStartKey, settings.conStartNoResetKey, settings.conTimer1Key, settings.conTimer2Key
	);
	
	// NEW: Add mappings for additional hotkeys
	if (settings.hotkeyClickThrough != 0) {
		hotkeysMap.insert({ settings.hotkeyClickThrough, KEY_CLICKTHROUGH_TOGGLE });
	}
	if (settings.hotkeyOpacityUp != 0) {
		hotkeysMap.insert({ settings.hotkeyOpacityUp, KEY_OPACITY_UP });
	}
	if (settings.hotkeyOpacityDown != 0) {
		hotkeysMap.insert({ settings.hotkeyOpacityDown, KEY_OPACITY_DOWN });
	}
}

void HotkeyManager::setHotkeysMap(int startKey, int startNoResetKey, int timer1Key, int timer2Key, int conStartKey, int conStartNoResetKey, int conTimer1Key, int conTimer2Key)
{
	hotkeysMap.clear();

	hotkeysMap.insert({ startKey, KEY_START });
	hotkeysMap.insert({ startNoResetKey, KEY_START_NO_RESET });
	hotkeysMap.insert({ timer1Key, KEY_TIMER1 });
	hotkeysMap.insert({ timer2Key, KEY_TIMER2 });

	hotkeysMap.insert({ conStartKey, KEY_START });
	hotkeysMap.insert({ conStartNoResetKey, KEY_START_NO_RESET });
	hotkeysMap.insert({ conTimer1Key, KEY_TIMER1 });
	hotkeysMap.insert({ conTimer2Key, KEY_TIMER2 });
}

void HotkeyManager::execute(const int keyCode)
{
	if (hotkeysMap.count(keyCode) && pGlobalTimerWindow->window() != nullptr)
	{
		const int action = hotkeysMap[keyCode];
		PostMessage(pGlobalTimerWindow->window(), HOTKEY_HIT, action, 0);
	}
}


#include "Globals.h"
#include "HotkeyManager.h"

#include "Program.h"

std::unordered_map<int, int> HotkeyManager::hotkeysMap;

void HotkeyManager::setHotkeysMap(const SettingsStruct& settings)
{
	setHotkeysMap(settings.startKey, settings.timer1Key, settings.timer2Key);
}

void HotkeyManager::setHotkeysMap(int startKey, int timer1Key, int timer2Key)
{
	hotkeysMap.clear();

	hotkeysMap.insert({ startKey, KEY_START });
	hotkeysMap.insert({ timer1Key, KEY_TIMER1 });
	hotkeysMap.insert({ timer2Key, KEY_TIMER2 });
}

void HotkeyManager::execute(const int keyCode)
{
	if (hotkeysMap.count(keyCode) && pGlobalTimerWindow->window() != nullptr)
	{
		const int action = hotkeysMap[keyCode];
		PostMessage(pGlobalTimerWindow->window(), HOTKEY_HIT, action, 0);
	}
}


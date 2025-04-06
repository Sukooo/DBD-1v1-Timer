#include "ControllerManager.h"

ControllerManager::ControllerManager()
{
	isRunning_ = false;
}

void ControllerManager::setInputCallback(const std::function<void(WORD)>& callback)
{
	inputCallback_ = callback;
}

void ControllerManager::poll()
{
	const DWORD result = XInputGetState(0, &state_);

	if (result == ERROR_SUCCESS)
	{
		const bool leftTriggerChanged = state_.Gamepad.bLeftTrigger / 255 != previousState_.Gamepad.bLeftTrigger / 255;
		const bool rightTriggerChanged = state_.Gamepad.bRightTrigger / 255 != previousState_.Gamepad.bRightTrigger / 255;
		const bool buttonsChanged = state_.Gamepad.wButtons != 0 && state_.Gamepad.wButtons != previousState_.Gamepad.wButtons;

		const bool isChange = buttonsChanged || (leftTriggerChanged || rightTriggerChanged);

		if (isChange)
		{
			WORD buttons = buttonsMap[state_.Gamepad.wButtons];

			if (state_.Gamepad.bLeftTrigger / 255 == 1)
			{
				buttons = ControllerButtons::LeftTrigger;
			}
			else if (state_.Gamepad.bRightTrigger / 255 == 1)
			{
				buttons = ControllerButtons::RightTrigger;
			}

			inputCallback_(buttons);
		}

		previousState_ = state_;
	}
}

void ControllerManager::start()
{
	if (isRunning_) return;

	isRunning_ = true;

	pollingThread_ = std::thread([this]()
	{
		while (isRunning_)
		{
			poll();
			Sleep(1);
		}
	});
}

void ControllerManager::stop()
{
	isRunning_ = false;
	if (pollingThread_.joinable())
	{
		pollingThread_.join();
	}
}

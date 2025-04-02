#pragma once
#include "MainWindow.h"

// Fields
extern MainWindow* pGlobalTimerWindow;

/*
@brief A method Running an infinite loop, responsible for updating the timers.
*/
extern void appLoop(MainWindow* win);

/*
@brief End the process of the program safely.
*/
extern void exitApp();

/*
@brief The hook procedure to listen for key strokes of the active HOTKEYS.

@param nCode Has no effect in this function but is part of the syntax.

@param wParam Contains the state of the hit key.

@param lParam Contains the key that was hit.

@return Forwards to the next hook.
*/
LRESULT CALLBACK kbHook(int nCode, WPARAM wParam, LPARAM lParam);

/*
@brief The hook procedure to listen for key strokes of the active HOTKEYS.

@param nCode Has no effect in this function but is part of the syntax.

@param wParam Contains the window message that was triggered.

@param lParam A pointer to a CWPRETSTRUCT structure that contains details about the message.

@return Forwards to the next hook.
*/
LRESULT CALLBACK mouseHook(int nCode, WPARAM wParam, LPARAM lParam);
#pragma once
#include "BaseWindow.h"

/*
@brief Set a specific control's font (title style).

@param hControl The control to change the font of.
*/
void setTitleFont(const HWND hControl);

/*
@brief Apply the default font to a specific window's child controls.

@param hWnd The handle to the window who's child controls will be affected.
*/
void setControlsFont(const HWND hWnd);

/*
@brief A procedure for windows of type "control", used to assign a font.

@param hControl The HWND control to assign the font to.

@param lParam An LPARAM containing the font to set for the hControl.

@return BOOL
*/
BOOL CALLBACK controlProc(const HWND hControl, const LPARAM lParam);
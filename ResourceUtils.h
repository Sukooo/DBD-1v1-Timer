#pragma once
#include "BaseWindow.h"

template <class T> void safeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

/*
@brief Load a bitmap resource.

@param bitmap The bitmap resource to load.

@return A handle to the bitmap loaded.
*/
HBITMAP loadBitmapResource(int bitmap);

/*
@brief Initialize the colors that can be chosen (as brushes).
*/
void initializeBrushes();
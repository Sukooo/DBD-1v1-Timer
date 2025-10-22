#include "ResourceUtils.h"

HBITMAP loadBitmapResource(const int bitmap) {
	const HBITMAP hBitmap = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(bitmap));
	if (!hBitmap) {
		MessageBox(nullptr, L"Failed to load bitmap!", L"Error", MB_OK);
	}

	return hBitmap;
}
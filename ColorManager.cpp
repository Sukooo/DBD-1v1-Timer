#include "ColorManager.h"

ColorManager::~ColorManager() {
    cleanup();
}

ColorManager& ColorManager::getInstance() {
    static ColorManager instance;
    return instance;
}

void ColorManager::initialize() {
    if (initialized_) {
        return; // Already initialized
    }

    // RGB values from ResourceUtils.cpp initializeBrushes()
    constexpr COLORREF colors[25] = {
        RGB(255, 0, 0), RGB(255, 77, 0), RGB(255, 116, 0), RGB(255, 154, 0), RGB(255, 193, 0),
        RGB(1, 55, 125), RGB(0, 157, 209), RGB(151, 231, 245), RGB(115, 211, 72), RGB(38, 177, 112),
        RGB(49, 0, 74), RGB(51, 0, 123), RGB(76, 0, 164), RGB(131, 0, 196), RGB(171, 0, 255),
        RGB(255, 0, 255), RGB(192, 64, 255), RGB(128, 128, 255), RGB(64, 182, 255), RGB(0, 255, 255),
        RGB(1, 1, 1), RGB(35, 35, 35), RGB(85, 85, 85), RGB(182, 176, 169), RGB(237, 231, 224)
    };

    for (int i = 0; i < 25; i++) {
        brushes_[i] = CreateSolidBrush(colors[i]);
    }

    initialized_ = true;
}

void ColorManager::cleanup() {
    if (!initialized_) {
        return;
    }

    for (int i = 0; i < 25; i++) {
        if (brushes_[i]) {
            DeleteObject(brushes_[i]);
            brushes_[i] = nullptr;
        }
    }

    initialized_ = false;
}

void ColorManager::refreshBrushes() {
    cleanup();
    initialize();
}

HBRUSH ColorManager::getBrush(int index) const {
    if (index < 0 || index >= 25) {
        return brushes_[0]; // Return first brush as default for invalid index
    }
    return brushes_[index];
}

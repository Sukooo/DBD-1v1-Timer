#pragma once
#include <Windows.h>

// Singleton class for managing application color brushes
class ColorManager {
public:
    static ColorManager& getInstance();

    void initialize();
    void cleanup();
    void refreshBrushes();
    HBRUSH getBrush(int index) const;

private:
    ColorManager() = default;
    ~ColorManager();
    ColorManager(const ColorManager&) = delete;
    ColorManager& operator=(const ColorManager&) = delete;

    HBRUSH brushes_[25]{};
    bool initialized_ = false;
};

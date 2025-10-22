#pragma once
#include <Windows.h>
#include <functional>
#include <unordered_map>

// Manages UI control bindings with lambda-based getters/setters
// Eliminates the need for giant switch statements
class ControlBindingManager {
public:
    explicit ControlBindingManager(HWND parentWindow);

    // Bind different control types
    void bindKeybind(int controlId, std::function<int()> getter, std::function<void(int)> setter);
    void bindCheckbox(int controlId, std::function<bool()> getter, std::function<void(bool)> setter);
    void bindSlider(int controlId, std::function<int()> getter, std::function<void(int)> setter);

    // Apply values between controls and settings
    void applyValueToControl(int controlId);
    void applyControlToValue(int controlId, int value);  // For keybinds
    void applyControlToValue(int controlId);  // For checkboxes/sliders

    // Update all controls from settings
    void updateAllControls();

    // Check if a control ID has a binding
    bool hasKeybindBinding(int controlId) const;
    bool hasCheckboxBinding(int controlId) const;
    bool hasSliderBinding(int controlId) const;

    // Get value from binding (for keybinds)
    int getKeybindValue(int controlId) const;

private:
    HWND parentWindow_;

    struct KeybindBinding {
        std::function<int()> getter;
        std::function<void(int)> setter;
    };

    struct CheckboxBinding {
        std::function<bool()> getter;
        std::function<void(bool)> setter;
    };

    struct SliderBinding {
        std::function<int()> getter;
        std::function<void(int)> setter;
    };

    std::unordered_map<int, KeybindBinding> keybindBindings_;
    std::unordered_map<int, CheckboxBinding> checkboxBindings_;
    std::unordered_map<int, SliderBinding> sliderBindings_;
};

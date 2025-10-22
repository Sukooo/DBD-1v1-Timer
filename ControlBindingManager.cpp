#include "ControlBindingManager.h"
#include <CommCtrl.h>

ControlBindingManager::ControlBindingManager(HWND parentWindow)
    : parentWindow_(parentWindow) {
}

void ControlBindingManager::bindKeybind(int controlId, std::function<int()> getter, std::function<void(int)> setter) {
    keybindBindings_[controlId] = { getter, setter };
}

void ControlBindingManager::bindCheckbox(int controlId, std::function<bool()> getter, std::function<void(bool)> setter) {
    checkboxBindings_[controlId] = { getter, setter };
}

void ControlBindingManager::bindSlider(int controlId, std::function<int()> getter, std::function<void(int)> setter) {
    sliderBindings_[controlId] = { getter, setter };
}

void ControlBindingManager::applyValueToControl(int controlId) {
    // Check checkbox bindings
    if (checkboxBindings_.count(controlId)) {
        bool value = checkboxBindings_[controlId].getter();
        HWND controlHwnd = GetDlgItem(parentWindow_, controlId);
        if (controlHwnd) {
            SendMessage(controlHwnd, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
        }
    }

    // Check slider bindings
    if (sliderBindings_.count(controlId)) {
        int value = sliderBindings_[controlId].getter();
        HWND controlHwnd = GetDlgItem(parentWindow_, controlId);
        if (controlHwnd) {
            SendMessage(controlHwnd, TBM_SETPOS, TRUE, value);
        }
    }

    // Keybind controls need external key name mapping in SettingsWindow
}

void ControlBindingManager::applyControlToValue(int controlId, int value) {
    // For keybinds - value is the virtual key code
    if (keybindBindings_.count(controlId)) {
        keybindBindings_[controlId].setter(value);
    }
}

void ControlBindingManager::applyControlToValue(int controlId) {
    // For checkboxes
    if (checkboxBindings_.count(controlId)) {
        HWND controlHwnd = GetDlgItem(parentWindow_, controlId);
        if (controlHwnd) {
            bool checked = (SendMessage(controlHwnd, BM_GETCHECK, 0, 0) == BST_CHECKED);
            checkboxBindings_[controlId].setter(checked);
        }
    }

    // For sliders
    if (sliderBindings_.count(controlId)) {
        HWND controlHwnd = GetDlgItem(parentWindow_, controlId);
        if (controlHwnd) {
            int value = static_cast<int>(SendMessage(controlHwnd, TBM_GETPOS, 0, 0));
            sliderBindings_[controlId].setter(value);
        }
    }
}

void ControlBindingManager::updateAllControls() {
    // Update all checkbox controls
    for (const auto& pair : checkboxBindings_) {
        applyValueToControl(pair.first);
    }

    // Update all slider controls
    for (const auto& pair : sliderBindings_) {
        applyValueToControl(pair.first);
    }

    // Keybind controls are updated separately in SettingsWindow with key name mapping
}

bool ControlBindingManager::hasKeybindBinding(int controlId) const {
    return keybindBindings_.count(controlId) > 0;
}

bool ControlBindingManager::hasCheckboxBinding(int controlId) const {
    return checkboxBindings_.count(controlId) > 0;
}

bool ControlBindingManager::hasSliderBinding(int controlId) const {
    return sliderBindings_.count(controlId) > 0;
}

int ControlBindingManager::getKeybindValue(int controlId) const {
    if (keybindBindings_.count(controlId)) {
        return keybindBindings_.at(controlId).getter();
    }
    return 0;
}

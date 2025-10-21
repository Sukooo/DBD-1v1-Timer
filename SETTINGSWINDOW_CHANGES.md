# SettingsWindow.cpp Required Changes

Due to file encoding issues, these manual changes are needed in SettingsWindow.cpp:

## 1. Remove Click-Through Checkbox

### In `initializeTextControls()` (around line 109):
**REMOVE:**
```cpp
const HWND hwndTextCheckboxClickthrough = createControl(WC_STATIC, L"Clickthrough (resets when app is closed)", titleX, tileHeight_ * 9, titleWidth, tileHeight_);
```

**REPLACE WITH:**
```cpp
// NEW: Click-through hotkey title (replacing checkbox)
const HWND hwndTextClickThroughToggle = createControl(WC_STATIC, L"Click-Through Toggle", titleX, tileHeight_ * 9, titleWidth, tileHeight_);

// NEW: Opacity controls titles
const HWND hwndTextOpacity = createControl(WC_STATIC, L"Opacity", titleX, tileHeight_ * 9 + 35, 100, tileHeight_);
const HWND hwndTextOpacityUp = createControl(WC_STATIC, L"Opacity +", titleX + 200, tileHeight_ * 9 + 10, 80, 20);
const HWND hwndTextOpacityDown = createControl(WC_STATIC, L"Opacity -", titleX + 200, tileHeight_ * 9 + 35, 80, 20);
```

### In `initializeButtonControls()` (around line 162):
**REMOVE:**
```cpp
const HWND hCbClickthrough = createControl(WC_BUTTON, L"", xCheckbox, tileHeight_ * 9, sizeCheckbox, sizeCheckbox, CID_CLICKTHROUGH_CB, BS_CHECKBOX | BS_AUTOCHECKBOX);
```

**REPLACE WITH:**
```cpp
// NEW: Click-through hotkey field (replaces checkbox)
const HWND hClickThroughHotkey = createControl(WC_BUTTON, L"", xHotkey, tileHeight_ * 9, widthHotkey, heightHotkey, CID_CLICKTHROUGH_HOTKEY, BS_FLAT);

// NEW: Opacity slider
HWND hOpacitySlider = CreateWindowEx(
    0, TRACKBAR_CLASS, L"",
    WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS,
    titleX, tileHeight_ * 9 + 50, 180, 30,
    hwnd_, (HMENU)CID_OPACITY_SLIDER, nullptr, nullptr);
SendMessage(hOpacitySlider, TBM_SETRANGE, TRUE, MAKELPARAM(0, 255));
SendMessage(hOpacitySlider, TBM_SETPOS, TRUE, tempSettings_.opacity);

// NEW: Opacity hotkey fields
const HWND hOpacityUpHotkey = createControl(WC_BUTTON, L"", titleX + 280, tileHeight_ * 9 + 10, 60, 20, CID_OPACITY_UP_HOTKEY, BS_FLAT);
const HWND hOpacityDownHotkey = createControl(WC_BUTTON, L"", titleX + 280, tileHeight_ * 9 + 35, 60, 20, CID_OPACITY_DOWN_HOTKEY, BS_FLAT);
```

**REMOVE (around line 185):**
```cpp
SendMessage(hCbClickthrough, BM_SETCHECK, appSettings.optionClickThrough, 0);
```

**ADD AFTER hotkey application loop:**
```cpp
// Apply currently set opacity hotkeys
SetWindowText(GetDlgItem(hwnd_, CID_CLICKTHROUGH_HOTKEY),
              getKeyName(tempSettings_.hotkeyClickThrough).c_str());
SetWindowText(GetDlgItem(hwnd_, CID_OPACITY_UP_HOTKEY),
              getKeyName(tempSettings_.hotkeyOpacityUp).c_str());
SetWindowText(GetDlgItem(hwnd_, CID_OPACITY_DOWN_HOTKEY),
              getKeyName(tempSettings_.hotkeyOpacityDown).c_str());
```

## 2. Remove Click-Through Checkbox Handler

### In `handleControlCommand()` (around line 262-264):
**REMOVE:**
```cpp
// Clickthrough checkbox
case CID_CLICKTHROUGH_CB:
    tempSettings_.optionClickThrough = (Button_GetCheck(hwndCtrl) == BST_CHECKED);
    break;
```

## 3. Add New Hotkey Handlers

### In `handleControlCommand()` - add to hotkey cases (around line 243):
**ADD:**
```cpp
case CID_CLICKTHROUGH_HOTKEY:
case CID_OPACITY_UP_HOTKEY:
case CID_OPACITY_DOWN_HOTKEY:
```

### Add WM_HSCROLL handler for opacity slider:
**ADD in handleMessage() switch statement:**
```cpp
case WM_HSCROLL:
{
    if ((HWND)lParam == GetDlgItem(hwnd_, CID_OPACITY_SLIDER)) {
        int pos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
        tempSettings_.opacity = pos;
        // Real-time preview
        SetLayeredWindowAttributes(hwndMainWindow, 0, pos, LWA_ALPHA);
    }
    return 0;
}
```

## 4. Update applyTempHotkey()

### Add new cases for new hotkeys (around line 383):
**ADD:**
```cpp
case CID_CLICKTHROUGH_HOTKEY:
    tempSettings_.hotkeyClickThrough = key;
    break;
case CID_OPACITY_UP_HOTKEY:
    tempSettings_.hotkeyOpacityUp = key;
    break;
case CID_OPACITY_DOWN_HOTKEY:
    tempSettings_.hotkeyOpacityDown = key;
    break;
```

## 5. Update applyHotkeySavedKey()

### Add new cases (around line 437):
**ADD:**
```cpp
case CID_CLICKTHROUGH_HOTKEY:
    if (keyboardMap_.count(tempSettings_.hotkeyClickThrough)) {
        SetWindowText(hCtrl, keyboardMap_[tempSettings_.hotkeyClickThrough]);
    }
    break;
case CID_OPACITY_UP_HOTKEY:
    if (keyboardMap_.count(tempSettings_.hotkeyOpacityUp)) {
        SetWindowText(hCtrl, keyboardMap_[tempSettings_.hotkeyOpacityUp]);
    }
    break;
case CID_OPACITY_DOWN_HOTKEY:
    if (keyboardMap_.count(tempSettings_.hotkeyOpacityDown)) {
        SetWindowText(hCtrl, keyboardMap_[tempSettings_.hotkeyOpacityDown]);
    }
    break;
```

## Summary
- Remove click-through checkbox completely (was runtime-only anyway)
- Add click-through toggle hotkey field
- Add opacity slider with real-time preview
- Add opacity up/down hotkey fields
- Update all hotkey handling methods to support new controls

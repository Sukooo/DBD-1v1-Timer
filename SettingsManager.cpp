#include "SettingsManager.h"
#include "Configuration.h"
#include "dist/json/json.h"
#include <fstream>

using namespace std;

SettingsManager& SettingsManager::getInstance() {
    static SettingsManager instance;
    return instance;
}

void SettingsManager::loadFromFile() {
    ifstream file(SETTINGS_FILE_NAME);
    Json::Value actualJson;
    Json::Reader reader;

    reader.parse(file, actualJson);

    // hotkeys
    if (actualJson["start"].isInt() && actualJson["timer1"].isInt() && actualJson["timer2"].isInt() &&
        actualJson["conStart"].isInt() && actualJson["conTimer1"].isInt() && actualJson["conTimer2"].isInt()
        && actualJson["startNoReset"].isInt() && actualJson["conStartNoReset"].isInt())
    {
        currentSettings_.startKey = actualJson["start"].asInt();
        currentSettings_.timer1Key = actualJson["timer1"].asInt();
        currentSettings_.timer2Key = actualJson["timer2"].asInt();
        currentSettings_.startNoResetKey = actualJson["startNoReset"].asInt();

        currentSettings_.conStartKey = actualJson["conStart"].asInt();
        currentSettings_.conTimer1Key = actualJson["conTimer1"].asInt();
        currentSettings_.conTimer2Key = actualJson["conTimer2"].asInt();
        currentSettings_.conStartNoResetKey = actualJson["conStartNoReset"].asInt();
    }

    // NEW: Additional hotkeys
    if (actualJson["hotkeyClickThrough"].isInt()) {
        currentSettings_.hotkeyClickThrough = actualJson["hotkeyClickThrough"].asInt();
    }
    if (actualJson["hotkeyOpacityUp"].isInt()) {
        currentSettings_.hotkeyOpacityUp = actualJson["hotkeyOpacityUp"].asInt();
    }
    if (actualJson["hotkeyOpacityDown"].isInt()) {
        currentSettings_.hotkeyOpacityDown = actualJson["hotkeyOpacityDown"].asInt();
    }

    // options
    if (actualJson["optionTransparent"].isBool() && actualJson["optionStartOnChange"].isBool()) {
        currentSettings_.optionTransparent = actualJson["optionTransparent"].asBool();
        currentSettings_.optionStartOnChange = actualJson["optionStartOnChange"].asBool();
    }

    // NEW: Opacity
    if (actualJson["opacity"].isInt()) {
        currentSettings_.opacity = actualJson["opacity"].asInt();
    } else {
        currentSettings_.opacity = Config::OpacityConfig::DEFAULT_OPACITY;
    }

    // colors
    Json::Value colors = actualJson["colors"];
    if (colors["timer"].isInt() && colors["selected timer"].isInt()
        && colors["last seconds"].isInt() && colors["background"].isInt())
    {
        currentSettings_.colors.timerColor = colors["timer"].asInt();
        currentSettings_.colors.selectedTimerColor = colors["selected timer"].asInt();
        currentSettings_.colors.lastSecondsColor = colors["last seconds"].asInt();
        currentSettings_.colors.backgroundColor = colors["background"].asInt();

        if (currentSettings_.colors.timerColor > 24 || currentSettings_.colors.selectedTimerColor > 24 ||
            currentSettings_.colors.lastSecondsColor > 24 || currentSettings_.colors.backgroundColor > 24)
        {
            currentSettings_.colors.timerColor = Config::DefaultColors::TIMER;
            currentSettings_.colors.selectedTimerColor = Config::DefaultColors::SELECTED_TIMER;
            currentSettings_.colors.lastSecondsColor = Config::DefaultColors::LAST_SECONDS;
            currentSettings_.colors.backgroundColor = Config::DefaultColors::BACKGROUND;
        }
    }
}

void SettingsManager::saveToFile() {
    ifstream file(SETTINGS_FILE_NAME);

    // Retrieve existing settings
    Json::Reader reader;
    Json::Value settingsJson;
    reader.parse(file, settingsJson);
    file.close();

    // Update json obj
    settingsJson["start"] = currentSettings_.startKey;
    settingsJson["timer1"] = currentSettings_.timer1Key;
    settingsJson["timer2"] = currentSettings_.timer2Key;
    settingsJson["startNoReset"] = currentSettings_.startNoResetKey;

    settingsJson["conStart"] = currentSettings_.conStartKey;
    settingsJson["conTimer1"] = currentSettings_.conTimer1Key;
    settingsJson["conTimer2"] = currentSettings_.conTimer2Key;
    settingsJson["conStartNoReset"] = currentSettings_.conStartNoResetKey;

    // NEW: Additional hotkeys
    settingsJson["hotkeyClickThrough"] = currentSettings_.hotkeyClickThrough;
    settingsJson["hotkeyOpacityUp"] = currentSettings_.hotkeyOpacityUp;
    settingsJson["hotkeyOpacityDown"] = currentSettings_.hotkeyOpacityDown;

    settingsJson["optionTransparent"] = currentSettings_.optionTransparent;
    settingsJson["optionStartOnChange"] = currentSettings_.optionStartOnChange;
    // NOTE: optionClickThrough is NOT saved (runtime-only)

    // NEW: Opacity
    settingsJson["opacity"] = currentSettings_.opacity;

    settingsJson["colors"]["timer"] = currentSettings_.colors.timerColor;
    settingsJson["colors"]["selected timer"] = currentSettings_.colors.selectedTimerColor;
    settingsJson["colors"]["last seconds"] = currentSettings_.colors.lastSecondsColor;
    settingsJson["colors"]["background"] = currentSettings_.colors.backgroundColor;

    // Write to file
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";

    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream outputFileStream(SETTINGS_FILE_NAME);
    writer->write(settingsJson, &outputFileStream);
}

void SettingsManager::createDefaultFile() {
    ofstream file(SETTINGS_FILE_NAME);

    ColorsStruct defaultColors;
    SettingsStruct defaultSettings;
    defaultSettings.colors = defaultColors;

    Json::Value settingsJson;

    settingsJson["start"] = defaultSettings.startKey;
    settingsJson["timer1"] = defaultSettings.timer1Key;
    settingsJson["timer2"] = defaultSettings.timer2Key;
    settingsJson["startNoReset"] = defaultSettings.startNoResetKey;

    settingsJson["conStart"] = defaultSettings.conStartKey;
    settingsJson["conTimer1"] = defaultSettings.conTimer1Key;
    settingsJson["conTimer2"] = defaultSettings.conTimer2Key;
    settingsJson["conStartNoReset"] = defaultSettings.conStartNoResetKey;

    // NEW: Additional hotkeys (default to 0 = none)
    settingsJson["hotkeyClickThrough"] = defaultSettings.hotkeyClickThrough;
    settingsJson["hotkeyOpacityUp"] = defaultSettings.hotkeyOpacityUp;
    settingsJson["hotkeyOpacityDown"] = defaultSettings.hotkeyOpacityDown;

    settingsJson["optionTransparent"] = defaultSettings.optionTransparent;
    settingsJson["optionStartOnChange"] = defaultSettings.optionStartOnChange;

    // NEW: Opacity
    settingsJson["opacity"] = defaultSettings.opacity;

    settingsJson["colors"]["timer"] = defaultSettings.colors.timerColor;
    settingsJson["colors"]["selected timer"] = defaultSettings.colors.selectedTimerColor;
    settingsJson["colors"]["last seconds"] = defaultSettings.colors.lastSecondsColor;
    settingsJson["colors"]["background"] = defaultSettings.colors.backgroundColor;

    // Write to file
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream outputFileStream(SETTINGS_FILE_NAME);
    writer->write(settingsJson, &outputFileStream);

    currentSettings_ = defaultSettings;
}

bool SettingsManager::fileExists() {
    const string name = SETTINGS_FILE_NAME;
    const ifstream f(name.c_str());
    return f.good();
}

const SettingsStruct& SettingsManager::getSettings() const {
    return currentSettings_;
}

void SettingsManager::updateSettings(const SettingsStruct& settings) {
    currentSettings_ = settings;
    saveToFile();

    // Emit event to notify listeners
    Event evt{ EventType::SettingsChanged, (void*)&currentSettings_ };
    EventManager::getInstance().emit(evt);
}

void SettingsManager::onEvent(const Event& event) {
    // SettingsManager can listen to events if needed in the future
}

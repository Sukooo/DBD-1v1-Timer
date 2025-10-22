#include "SettingsManager.h"
#include "Configuration.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

SettingsManager& SettingsManager::getInstance() {
    static SettingsManager instance;
    return instance;
}

void SettingsManager::loadFromFile() {
    try {
        std::ifstream file(SETTINGS_FILE_NAME);
        if (!file.is_open()) {
            // File doesn't exist, use defaults
            currentSettings_ = AppSettings();
            return;
        }

        json j;
        file >> j;
        currentSettings_ = j.get<AppSettings>();

        // Validate color indices
        if (currentSettings_.colors.timerColor > 24 || currentSettings_.colors.selectedTimerColor > 24 ||
            currentSettings_.colors.lastSecondsColor > 24 || currentSettings_.colors.backgroundColor > 24) {
            currentSettings_.colors.timerColor = Config::DefaultColors::TIMER;
            currentSettings_.colors.selectedTimerColor = Config::DefaultColors::SELECTED_TIMER;
            currentSettings_.colors.lastSecondsColor = Config::DefaultColors::LAST_SECONDS;
            currentSettings_.colors.backgroundColor = Config::DefaultColors::BACKGROUND;
        }
    }
    catch (const std::exception&) {
        // On error, use defaults
        currentSettings_ = AppSettings();
    }
}

void SettingsManager::saveToFile() {
    try {
        json j = currentSettings_;
        std::ofstream file(SETTINGS_FILE_NAME);
        file << j.dump(3); // Pretty print with 3-space indent (matches old format)
    }
    catch (const std::exception&) {
        // Handle error silently (could add logging here)
    }
}

void SettingsManager::createDefaultFile() {
    currentSettings_ = AppSettings(); // Use default constructor
    saveToFile();
}

bool SettingsManager::fileExists() {
    std::ifstream file(SETTINGS_FILE_NAME);
    return file.good();
}

const AppSettings& SettingsManager::getSettings() const {
    return currentSettings_;
}

void SettingsManager::updateSettings(const AppSettings& settings) {
    currentSettings_ = settings;
    saveToFile();
    
    // Emit event to notify listeners
    Event evt{ EventType::SettingsChanged, (void*)&currentSettings_ };
    EventManager::getInstance().emit(evt);
}

void SettingsManager::onEvent(const Event& event) {
    // SettingsManager can listen to events if needed in the future
}

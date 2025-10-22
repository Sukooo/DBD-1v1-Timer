#pragma once
#include "AppSettings.h"
#include "EventManager.h"
#include <string>

// Centralized settings management with event emission
class SettingsManager : public IEventListener {
public:
    static SettingsManager& getInstance();

    void loadFromFile();
    void saveToFile();
    void createDefaultFile();
    bool fileExists();

    const AppSettings& getSettings() const;
    void updateSettings(const AppSettings& settings);  // Emits SettingsChanged event

    void onEvent(const Event& event) override;

private:
    SettingsManager() = default;
    AppSettings currentSettings_;
    
    static constexpr const char* SETTINGS_FILE_NAME = "Settings.json";
};

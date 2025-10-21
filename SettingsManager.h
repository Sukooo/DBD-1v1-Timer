#pragma once
#include "globals.h"
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

    const SettingsStruct& getSettings() const;
    void updateSettings(const SettingsStruct& settings);  // Emits SettingsChanged event

    void onEvent(const Event& event) override;

private:
    SettingsManager() = default;
    SettingsStruct currentSettings_;
};

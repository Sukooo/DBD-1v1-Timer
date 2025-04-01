#pragma once

#include "Globals.h"

/*
@brief Safely retrieves the SettingsStruct from the settings.json file.
		Handles errors in the settings.json file and resets it if need be.

@return The SettingsStruct representing the saved settings from the settings.json file.
*/
SettingsStruct getSafeSettingsStruct();

/*
@brief Writes the settings from the given SettingsStruct to the settings.json file.

@param settings The settings struct to save to the file.
*/
void setSettingsStruct(const SettingsStruct& settings);

/*
@brief Initial creation of the settings.json file.
*/
void createSettingsFile();

/*
@return Wether settings.json exists or not.
*/
bool settingsFileExists();

/*
@brief Save settings to local variable and json file (also apply temporary settings).

@param settings The settings to be applied.
*/
void applySettings(const SettingsStruct& settings);

#include <fstream>
#include "SettingsUtils.h"

#include "HotkeyManager.h"
#include "dist/json/json.h"

using namespace std;

SettingsStruct getSafeSettingsStruct()
{
	ifstream file(SETTINGS_FILE_NAME);
	Json::Value actualJson;
	Json::Reader reader;
	SettingsStruct settings;

	reader.parse(file, actualJson);

	// hotkeys
	if (actualJson["start"].isInt() && actualJson["timer1"].isInt() && actualJson["timer2"].isInt())
	{
		settings.startKey = actualJson["start"].asInt();
		settings.timer1Key = actualJson["timer1"].asInt();
		settings.timer2Key = actualJson["timer2"].asInt();
	}

	// options
	if (actualJson["optionTransparent"].isBool() && actualJson["optionStartOnChange"].isBool()) {
		settings.optionTransparent = actualJson["optionTransparent"].asBool();
		settings.optionStartOnChange = actualJson["optionStartOnChange"].asBool();
	}

	settings.optionClickThrough = false;

	// colors
	Json::Value colors = actualJson["colors"];
	if (colors["timer"].isInt() && colors["selected timer"].isInt()
		&& colors["last seconds"].isInt() && colors["background"].isInt())
	{
		settings.colors.timerColor = colors["timer"].asInt();
		settings.colors.selectedTimerColor = colors["selected timer"].asInt();
		settings.colors.lastSecondsColor = colors["last seconds"].asInt();
		settings.colors.backgroundColor = colors["background"].asInt();

		if (settings.colors.timerColor > 24 || settings.colors.selectedTimerColor > 24 ||
			settings.colors.lastSecondsColor > 24 || settings.colors.backgroundColor > 24)
		{
			settings.colors.timerColor = 9;
			settings.colors.selectedTimerColor = 6;
			settings.colors.lastSecondsColor = 1;
			settings.colors.backgroundColor = 20;
		}
	}

	return settings;
}

void setSettingsStruct(const SettingsStruct& settings)
{
	ifstream file(SETTINGS_FILE_NAME);

	// Retrieve existing settings
	Json::Reader reader;
	Json::Value settingsJson;
	reader.parse(file, settingsJson);

	// Change json obj
	settingsJson["start"] = settings.startKey;
	settingsJson["timer1"] = settings.timer1Key;
	settingsJson["timer2"] = settings.timer2Key;
	settingsJson["optionTransparent"] = settings.optionTransparent;
	settingsJson["optionStartOnChange"] = settings.optionStartOnChange;
	settingsJson["colors"]["timer"] = settings.colors.timerColor;
	settingsJson["colors"]["selected timer"] = settings.colors.selectedTimerColor;
	settingsJson["colors"]["last seconds"] = settings.colors.lastSecondsColor;
	settingsJson["colors"]["background"] = settings.colors.backgroundColor;

	// Write to file
	Json::StreamWriterBuilder builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "   ";

	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	std::ofstream outputFileStream(SETTINGS_FILE_NAME);
	writer->write(settingsJson, &outputFileStream);
}

void createSettingsFile()
{
	ofstream file(SETTINGS_FILE_NAME);

	ColorsStruct defaultColors;

	SettingsStruct defaultSettings;
	defaultSettings.colors = defaultColors;

	Json::Value settingsJson;
	
	settingsJson["start"] = defaultSettings.startKey;
	settingsJson["timer1"] = defaultSettings.timer1Key;
	settingsJson["timer2"] = defaultSettings.timer2Key;
	settingsJson["optionTransparent"] = defaultSettings.optionTransparent;
	settingsJson["optionStartOnChange"] = defaultSettings.optionStartOnChange;
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
}

bool settingsFileExists() {
	const string name = SETTINGS_FILE_NAME;

	const ifstream f(name.c_str());
	return f.good();
}

void applySettings(const SettingsStruct& settings) {
	setSettingsStruct(settings); // write to json file (settings.json)
	appSettings = settings; // save static settings variable
	HotkeyManager::setHotkeysMap(appSettings); // initialize hotkeys map

	if (hwndMainWindow != nullptr) {
		// transparency
		if (appSettings.optionTransparent) { // add transparent effect
			SetLayeredWindowAttributes(hwndMainWindow, 0, 0, LWA_COLORKEY);
		}
		else { // remove transparent effect
			SetLayeredWindowAttributes(hwndMainWindow, 0, 255, LWA_ALPHA);
		}

		// click through
		if (appSettings.optionClickThrough) { // make click through
			// Get the current window style
			LONG style = GetWindowLong(hwndMainWindow, GWL_EXSTYLE);

			// Add the new style to the current styles
			style |= WS_EX_TRANSPARENT;

			// Set the new style
			SetWindowLong(hwndMainWindow, GWL_EXSTYLE, style);
		}
	}
}

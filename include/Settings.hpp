#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <stdlib.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "CommonUtils.hpp"

using json = nlohmann::json;

#define PROJECT_NAME "smile"
#define SETTINGS_FILE_NAME "settings.json"

#define CONFIG_INDENTATION_SIZE 4

#define DEFAULT_DATABASE_HISTORY_STORAGE false

/**
 * @class Settings
 * @brief Manages the loading and retrieval of application settings from a JSON configuration file.
 */
class Settings {

private:

    const std::string projectName = std::string(PROJECT_NAME);
    const std::string userHomePath = std::string(getenv("HOME"));
    const std::string settingsFileName = std::string(SETTINGS_FILE_NAME);
    const std::filesystem::path settingsDirectoryPath = userHomePath + "/." + projectName;

    json settingsFile;

    bool databaseHistoryStorage;

public:

    Settings() {     

        bool directoryExists = std::filesystem::is_directory(settingsDirectoryPath);
        if (!directoryExists) {
            spdlog::warn("Configuration directory does not exist in path: " + settingsDirectoryPath.string());
            spdlog::info("Generating new config folder...");

            std::filesystem::create_directory(settingsDirectoryPath);

            spdlog::info("Directory " + settingsDirectoryPath.string() + " successfully generated");
            spdlog::info("Generating settings.json file...");

            settingsFile["databaseHistoryStorage"] = DEFAULT_DATABASE_HISTORY_STORAGE;
            std::ofstream file(settingsDirectoryPath.string() + "/" + settingsFileName);
            file<<settingsFile;

            file.close();
        } else {
            spdlog::info("Loding configuration...");

            std::ifstream file(settingsDirectoryPath.string() + "/" + settingsFileName);
            if (!file.is_open()) {
                spdlog::error("Error while opening settings file: " +  settingsDirectoryPath.string() + "/" + settingsFileName);
                exit(1);
            }

            file>>settingsFile;

            databaseHistoryStorage = settingsFile["databaseHistoryStorage"].get<bool>();

            if (databaseHistoryStorage) {
                std::string databaseFilePath = settingsDirectoryPath.string() + "/" + "historyStorage.db";

                try {

                    // If there is no sqlite database file in the settings folder, generate it.
                    // Either way, establishes a connection
                    SQLite::Database db("testdb/historyStorage.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
                }
                catch (const std::exception& e) {
                    spdlog::error("Error creating database: ", e.what());
                }
                
            }

            file.close();
        }
        spdlog::info("Settings file successfully loaded");
        
    }




    //Getter methods
    std::string getProjectName() { return projectName; }
    std::string getUserHomePath() { return userHomePath; }
    std::string getSettingsFileName() { return settingsFileName; }
    std::string getSettingsDirectoryPath() { return settingsDirectoryPath; }
    
    json getSettingsFile() { return settingsFile; }
};
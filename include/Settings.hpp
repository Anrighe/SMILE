#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <stdlib.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "CommonUtils.hpp"
#include "DatabaseStatements.hpp"

using json = nlohmann::json;

#define PROJECT_NAME "smile"
#define SETTINGS_FILE_NAME "settings.json"
#define CONFIG_INDENTATION_SIZE 4
#define DATABASE_FILENAME "historyStorage.db"
#define DEFAULT_DATABASE_HISTORY_STORAGE true

#define DEBUG false

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
    const std::filesystem::path settingsFilePath = settingsDirectoryPath.string() + "/" + settingsFileName;

    json settingsFile;

    bool databaseHistoryStorage;
    SQLite::Database * db;

    void generateSettingsDirectory() {
        spdlog::info("Generating new settings directory...");
        std::filesystem::create_directory(settingsDirectoryPath);
        spdlog::info("Directory " + settingsDirectoryPath.string() + " successfully generated");
    }

    void generateSettingsDirectoryIfNotExists() {
        bool directoryExists = std::filesystem::is_directory(settingsDirectoryPath);
        if (!directoryExists) {
            spdlog::warn("Configuration directory does not exist in path: " + settingsDirectoryPath.string());
            generateSettingsDirectory();
        }
    }

    void generateSettingsFile() {
        spdlog::info("Generating settings.json file...");

        std::string enabledDatabaseMessage;
        enabledDatabaseMessage = DEFAULT_DATABASE_HISTORY_STORAGE ? "Enabling" : "Disabling";     
        spdlog::info(enabledDatabaseMessage.append(" database by default in the settings file..."));
        settingsFile["databaseHistoryStorage"] = DEFAULT_DATABASE_HISTORY_STORAGE;

        // Storing each path in the system $PATH variable in the systemPathVariableListed vector
        spdlog::info("Loading by default all path contained in the system Path variable as a lookup reference in the settings file...");
        std::string systemPathVariable = getenv("PATH");
        std::stringstream ss(systemPathVariable);
        std::vector<std::string> systemPathVariableListed;
        std::string tmpVariable;

        while(getline(ss, tmpVariable, ':'))
            systemPathVariableListed.push_back(tmpVariable);
        
        settingsFile["systemBinariesPath"] = systemPathVariableListed;
        std::ofstream file(settingsFilePath);
        file<<settingsFile;

        file.close();
    }

    void generateSettingsFileIfNotExists() {
        bool settingsFileExists = std::filesystem::is_regular_file(settingsFilePath);
        if (!settingsFileExists)
            generateSettingsFile();
    }

public:

    Settings() {     

        generateSettingsDirectoryIfNotExists();

        generateSettingsFileIfNotExists();

        spdlog::info("Loding configuration...");

        std::ifstream file(settingsFilePath);
        if (!file.is_open()) {
            spdlog::error("Error while opening settings file: " +  settingsDirectoryPath.string() + "/" + settingsFileName);
            exit(1);
        }

        file>>settingsFile;

        file.close();

        databaseHistoryStorage = settingsFile["databaseHistoryStorage"].get<bool>();
        spdlog::info("Database history storage enabled: {}", databaseHistoryStorage);
        if (databaseHistoryStorage) {

            #if DEBUG == true
                std::string databaseFilePath = "historyStorage.db";
            #else
                std::string databaseFilePath = settingsDirectoryPath.string() + "/" + DATABASE_FILENAME;
            #endif

            try {

                // If there is no sqlite database file in the settings directory, generates it.
                // Either way, establishes a connection
                db = new SQLite::Database(databaseFilePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

                spdlog::info("Established connection to history database file in: {}", databaseFilePath);

                // Connects the Database Statement module to the connected database
                DatabaseStatements databaseStatements(*db);

                spdlog::info("Instantiated database statements module");

                // Instantiates history table if not exists
                SQLite::Statement query = databaseStatements.getCreateHistoryTableIfNotExistsPreparedQuery();
                query.exec();

                spdlog::info("Database history table existance established");

            }
            catch (const SQLite::Exception& e) {
                spdlog::error("Error during query execution {}", e.what());
            }
            catch (const std::exception& e) {
                spdlog::error("Error handling database: {}", e.what());
            }
            
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
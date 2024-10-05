#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <stdlib.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <unistd.h>
#include "CommonUtils.hpp"
#include "DatabaseStatements.hpp"

using json = nlohmann::json;

#define PROJECT_NAME "smile"
#define SETTINGS_FILE_NAME "settings.json"
#define CONFIG_INDENTATION_SIZE 4
#define DATABASE_FILENAME "historyStorage.db"
#define DEFAULT_DATABASE_HISTORY_STORAGE true
#define DEFAULT_IGNORE_MNT_FROM_SYSTEM_PATH_VARIABLES true
#define DEFAULT_LENGTH_CONDITION_ENABLED true
#define DEFAULT_LENGTH_CONDITION_HEURISTIC 2

#define DEBUG false

/**
 * @class Settings
 * @brief Manages the loading and retrieval of application settings from a JSON configuration file.
 */
class Settings {

private:

    const std::string projectName = std::string(PROJECT_NAME);
    const std::string settingsFileName = std::string(SETTINGS_FILE_NAME);
    const std::filesystem::path userHomePath = std::string(getenv("HOME"));
    const std::filesystem::path settingsDirectoryPath = userHomePath.string() + "/." + projectName;
    const std::filesystem::path settingsFilePath = settingsDirectoryPath.string() + "/" + settingsFileName;
    const std::filesystem::path databaseFilePath = settingsDirectoryPath.string() + "/" + DATABASE_FILENAME;

    json settingsFile;

    bool databaseHistoryStorageEnabled;
    bool ignoreMntFromSystemPathVariables;
    bool lengthConditionHeuristicEnabled;
    int lengthConditionHeuristic;
    std::vector<std::string> systemPathVariableList;
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
        settingsFile["databaseHistoryStorageEnabled"] = DEFAULT_DATABASE_HISTORY_STORAGE;

        // Storing each path in the system $PATH variable in the systemPathVariableListed vector
        spdlog::info("Loading by default all path contained in the system Path variable as a lookup reference in the settings file...");
        std::string systemPathVariable = getenv("PATH");
        std::stringstream ss(systemPathVariable);
        std::vector<std::string> systemPathVariableListed;
        std::string tmpVariable;

        while(getline(ss, tmpVariable, ':'))
            systemPathVariableListed.push_back(tmpVariable);
        
        settingsFile["systemBinariesPath"] = systemPathVariableListed;
        
        settingsFile["ignoreMntFromSystemPathVariables"] = DEFAULT_IGNORE_MNT_FROM_SYSTEM_PATH_VARIABLES;

        settingsFile["lengthConditionHeuristicEnabled"] = DEFAULT_LENGTH_CONDITION_ENABLED;
        settingsFile["lengthConditionHeuristic"] = DEFAULT_LENGTH_CONDITION_HEURISTIC;

        std::ofstream file(settingsFilePath);
        file<<settingsFile;
        file.close();
    }

    void generateSettingsFileIfNotExists() {
        bool settingsFileExists = std::filesystem::is_regular_file(settingsFilePath);
        if (!settingsFileExists)
            generateSettingsFile();
    }

    void loadSettingsFile() {
        try {
            spdlog::info("Loding settings file configuration...");

            std::ifstream file(settingsFilePath);
            if (!file.is_open()) {
                spdlog::error("Error while opening settings file: " +  settingsDirectoryPath.string() + "/" + settingsFileName);
                exit(1);
            }

            file>>settingsFile;
            file.close();

            ignoreMntFromSystemPathVariables = settingsFile["ignoreMntFromSystemPathVariables"].get<bool>();
            systemPathVariableList = settingsFile["systemBinariesPath"].get<std::vector<std::string>>();
            lengthConditionHeuristicEnabled = settingsFile["lengthConditionHeuristicEnabled"].get<bool>();
            lengthConditionHeuristic = settingsFile["lengthConditionHeuristic"].get<int>();

            databaseHistoryStorageEnabled = settingsFile["databaseHistoryStorageEnabled"].get<bool>();
            generateDatabaseIfNotExists(databaseHistoryStorageEnabled);
            
            spdlog::info("Settings file successfully loaded");
        } catch (const std::exception &e) {
            spdlog::error("Error while loading the settings file: {}", e.what());
        }
    }

    void generateDatabaseIfNotExists(bool databaseHistoryStorageEnabled) {
        if (databaseHistoryStorageEnabled) {
            spdlog::info("Database history storage enabled");
            try {
                // If there is no sqlite database file in the settings directory, generates it.
                // Either way, establishes a connection
                db = new SQLite::Database(databaseFilePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

                spdlog::info("Established connection to history database file in: {}", databaseFilePath.string());

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
        } else 
            spdlog::info("Database history storage disabled");
    }

public:

    Settings() {     

        generateSettingsDirectoryIfNotExists();

        generateSettingsFileIfNotExists();

        loadSettingsFile();
    }

    // Getter methods
    std::string getProjectName() { return projectName; }
    std::string getSettingsFileName() { return settingsFileName; }

    std::string getUserHomePathStringString() { return userHomePath.string(); }
    std::string getSettingsDirectoryPathString() { return settingsDirectoryPath.string(); }
    std::string getSettingsFilePathString() { return settingsFilePath.string(); }
    std::string getDatabaseFilePathString() { return databaseFilePath.string(); }

    std::filesystem::path getUserHomePath() { return userHomePath; }
    std::filesystem::path getSettingsDirectoryPath() { return settingsDirectoryPath; }
    std::filesystem::path getSettingsFilePath() { return settingsFilePath; }
    std::filesystem::path getDatabaseFilePath() { return databaseFilePath; }
    
    json getSettingsFile() { return settingsFile; }

    std::vector<std::string> getSystemPathVariablePaths() { 
        if (ignoreMntFromSystemPathVariables) {
            std::vector<std::string> filteredSystemPathVariableList;
            for (const auto& entry : systemPathVariableList) {
                if (entry.find("/mnt") != 0)
                    filteredSystemPathVariableList.push_back(entry);
            }
            return filteredSystemPathVariableList;
        } else
            return systemPathVariableList; 
    }

    // By adding const to these member functions, it promises that calling them will not change the state of the Settings object
    bool getLengthConditionHeuristicEnabled() const { return lengthConditionHeuristicEnabled; }
    int getLengthConditionHeuristic() const { return lengthConditionHeuristic; }
};
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

#define DEFAULT_DATABASE_HISTORY_STORAGE true

#define DEBUG true

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
    SQLite::Database * db;

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

            file.close();
        }
            databaseHistoryStorage = settingsFile["databaseHistoryStorage"].get<bool>();
            spdlog::info("Database history storage enabled: {}", databaseHistoryStorage);
            if (databaseHistoryStorage) {

                #if DEBUG == true
                    std::string databaseFilePath = "historyStorage.db";
                #else
                    std::string databaseFilePath = settingsDirectoryPath.string() + "/" + "historyStorage.db";
                #endif

                try {

                    // If there is no sqlite database file in the settings folder, generates it.
                    // Either way, establishes a connection
                    db = new SQLite::Database(databaseFilePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

                    spdlog::info("Established connection to history database file in: {}", databaseFilePath);

                    // Connects the Database Statement module to the connected database
                    DatabaseStatements databaseStatements(*db);

                    spdlog::info("Instantiated database statements module");


                    spdlog::info("CREATING QUERY");
                    SQLite::Statement query = databaseStatements.createHistoryTable();

                    spdlog::info("EXECUTING QUERY");
                    while (query.executeStep()) {
                        std::cout<<query.getColumn(0)<<"\n";
                    }



                    /*db->exec("CREATE TABLE IF NOT EXISTS history (id INTEGER PRIMARY KEY, event TEXT)");

                    spdlog::info("Rows affecte by the SELECT * FROM history statement: {}",db->exec("SELECT * FROM history"));

                    db->exec("INSERT INTO TABLE history(1, 'WOW')");

                    db->exec("CREATE TABLE IF NOT EXISTS history (id INTEGER PRIMARY KEY, event TEXT)");

                    spdlog::info("Rows affecte by the SELECT * FROM history statement: {}",db->exec("SELECT * FROM history"));*/
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
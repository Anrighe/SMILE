#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <stdlib.h>

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
                // TODO:
                // Check database existance
                //  If not present, generate it

                // Finally (in either case), establish a connection 
            }

            file.close();
        }
        spdlog::info("Settings file successfully loaded");
        
    }

    /**
     * @brief Checks if a string field exists and is not empty in a JSON object.
     *
     * Verifies whether a specified field exists in the given JSON object and that it is a non-empty string.
     * If the field is valid, the value is assigned to the provided reference variable.
     *
     * @param data The JSON object to check.
     * @param field The name of the field to check.
     * @param valueToAssign Reference to the variable where the field's value will be assigned.
     * @return True if the field exists and is a non-empty string; false otherwise.
     */
    bool doesStringExistAndIsNotEmptyInJson(json data, std::string field, std::string &valueToAssign) {
        if (data.contains(field) && data[field].is_string() && data[field] != "") {
            valueToAssign = data[field];
            return true;
        } else {
            spdlog::error(field + " is not present in the settings file or has not been set");
            return false;
        }
    }

    /**
     * @brief Validates whether the provided string represents a valid non-negative integer.
     *
     * @param indentationSize the string representing to be validated
     * @return ```true``` if the string can be successfully converted to a non-negative integer, ```false``` otherwise
     * @throws ```std::exception``` if the string cannot be converted to an integer (either due to an invalid format or value out of range)
     */
    bool isStringAnIntegerGreaterOrEqualThanZero(const std::string indentationSize) {
        try {
            int indentSize = std::stoi(indentationSize);
            if (indentSize >= 0)
                return true;
        } catch (const std::exception &e) {
            spdlog::error("Could not validate the settings parameter 'indentationSize'");
            std::cerr<<e.what()<<"\n";
            return false;
        }
        return false;
    }

    /**
     * @brief Converts the settings object indentation size into a string of whitespace characters.
     *
     * @return a string of whitespace characters with a length corresponding to the integer value of ```indentationSize```
     * @throws ```std::invalid_argument``` if the ```indentationSize``` cannot be converted to an integer
     * @throws ```std::out_of_range``` if the ```indentationSize``` value is out of range for an integer
     */
    std::string getIndentationSizeInWhitespaces() {
        std::string indentation = "";
        for (std::size_t i = 0; i < CONFIG_INDENTATION_SIZE; ++i)
            indentation += " ";
        return indentation;
    }

    //Getter methods
    std::string getProjectName() { return projectName; }
    std::string getUserHomePath() { return userHomePath; }
    std::string getSettingsFileName() { return settingsFileName; }
    std::string getSettingsDirectoryPath() { return settingsDirectoryPath; }
    
    json getSettingsFile() { return settingsFile; }
};
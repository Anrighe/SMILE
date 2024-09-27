#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CommonUtils {

    static const u_int8_t configIndentationSize = 4;

private:

public:

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
    static bool doesStringExistAndIsNotEmptyInJson(json data, std::string field, std::string &valueToAssign) {
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
    static bool isStringAnIntegerGreaterOrEqualThanZero(const std::string indentationSize) {
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
    static std::string getIndentationSizeInWhitespaces() {
        std::string indentation = "";
        for (std::size_t i = 0; i < configIndentationSize; ++i)
            indentation += " ";
        return indentation;
    }

    static bool doesCurrentUserHaveExecutablePermissionForFile(const std::filesystem::path& filePath) {
        return std::filesystem::is_regular_file(filePath) && (access(filePath.c_str(), X_OK) == 0);
    }

    static std::vector<std::string> getListOfFilesInPath(std::string path, bool recursive, bool executablePermission) {
        std::vector<std::string> fileList;

        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            if (recursive && entry.is_directory()) {
                std::vector<std::string> tmpVector = getListOfFilesInPath(entry.path().string(), recursive, executablePermission);
                try {
                    fileList.insert(fileList.end(), tmpVector.begin(), tmpVector.end());
                } catch (std::filesystem::__cxx11::filesystem_error &e) {
                    spdlog::warn("Error while opening {}: {}. Ignoring...", entry.path().string(), e.what());
                }
            } else {
                try {
                    if (executablePermission && doesCurrentUserHaveExecutablePermissionForFile(entry.path()))
                        fileList.push_back(entry.path().filename().string());
                } catch (std::filesystem::__cxx11::filesystem_error &e) {
                    spdlog::warn("Error while opening {}: {}. Ignoring...", entry.path().string(), e.what());
                }
            }
        }

        return fileList;
    }

    /**
     * Prints the contents of a vector.
     * This function works with vectors of any type that can be sent to the output stream.
     * 
     * @tparam T The type of the elements in the vector.
     * @param vector A reference to the vector to be printed.
     */
    template <typename T>
    static void printVector(const std::vector<T>& vector) {
        for (const auto& element : vector)
            std::cout<<element<<" ";
        std::cout<<"\n";
    }

    /**
     * Prints the contents of a set.
     * This function works with sets of any type that can be sent to the output stream.
     * 
     * @tparam T The type of the elements in the set.
     * @param set A reference to the set to be printed.
     */
    template <typename SetType>
    static void printSet(const SetType& set) {
        for (const auto& element : set)
            std::cout<<element<<" ";
        std::cout<<"\n";
    }

    static std::unordered_set<char> getSetOfUniqueCharFromString(std::string string) {
        std::unordered_set<char> charSet;
        for(std::size_t i = 0; i < string.size(); ++i)
            charSet.insert(string[i]);

        return charSet;
    }
};
#include <iostream>
#include <map>
#include <set>
#include <cmath>
#include <unordered_set>
#include "../include/Settings.hpp"
#include "../include/WordDistanceHandler.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

/**
 * Compares the given input command against binaries found in the system path, applying, if enabled a heuristic 
 * based on the difference in length and character similarities between the input command and system binaries, 
 * which filters those whose difference with the input command exceeds a threshold.
 * Calculates the word distances with the Damerau-Leveshtein algorithm and suggests the closest matches. 
 * 
 * @param vm Boost program options variables map that holds command line options.
 * @param inputCommand The command entered by the user, which needs to be matched or corrected.
 * @param settings The settings object containing configuration such as system path variables and heuristics for matching.
 * @return true if similar commands are found, otherwise false if no suggestions can be made.
 */
bool suggestCommands(const po::variables_map vm, std::string inputCommand, Settings settings) {
    spdlog::info("Printing content of system path vector");

    // If verbose mode enabled print the content of the system path vector
    if (vm.count("v"))
        CommonUtils::printVector(settings.getSystemPathVariablePaths());

    // Moving system path to a new data structure
    std::set<std::string> systemPathVariableSet;
    for (const auto &entry : settings.getSystemPathVariablePaths()) {
        std::vector<std::string> filesInPath = CommonUtils::getListOfFilesInPath(entry, false, true);
        systemPathVariableSet.insert(filesInPath.begin(), filesInPath.end());
    }

    spdlog::info("Printing content of system path set");

    // If verbose mode enabled print the set
    if (vm.count("v"))
        CommonUtils::printSet(systemPathVariableSet);

    spdlog::info("Before filtering set size: {}", systemPathVariableSet.size());

    std::set<std::string> systemPathVariableFilteredSet;

    spdlog::info("Applying length distance heuristic based on a maximum difference in length of {}", settings.getLengthConditionHeuristic());
    if (settings.getLengthConditionHeuristicEnabled()) {

        std::copy_if(
            systemPathVariableSet.begin(), 
            systemPathVariableSet.end(), 
            std::inserter(systemPathVariableFilteredSet, systemPathVariableFilteredSet.end()), 
            [inputCommand, settings](const std::string& value){
                
                int absoluteLengthLetterDifference = std::abs(static_cast<int>(value.length()) - static_cast<int>(inputCommand.length()));    

                // This heuristic is used to improve runtimes by reducing the numbers of binaries to check based on the total amount of
                // length differencies between the two words. By default, if their length is different by two or more letters, their
                // distance do not get calculated
                bool lengthCondition = absoluteLengthLetterDifference <= settings.getLengthConditionHeuristic();

                if (!lengthCondition)
                    return lengthCondition;

                std::unordered_set<char> inputCommandCharSet = CommonUtils::getSetOfUniqueCharFromString(inputCommand);
                std::unordered_set<char> currentBinaryCharSet = CommonUtils::getSetOfUniqueCharFromString(value);
                
                std::unordered_set<char> intersectionSet;
                for (char ch : inputCommandCharSet) {
                    if (currentBinaryCharSet.find(ch) != currentBinaryCharSet.end())
                        intersectionSet.insert(ch);
                }
                
                bool letterCondition = intersectionSet.size() >= (inputCommandCharSet.size() / 2);

                return letterCondition;
            }
        );
    } else
        systemPathVariableFilteredSet = systemPathVariableSet;

    if (settings.getLengthConditionHeuristicEnabled()) {
        spdlog::info("After filtering set size: {}", systemPathVariableFilteredSet.size());

        spdlog::info("Printing content of system path set after filtering");
        // If verbose mode enabled print the newly filtered set
        if (vm.count("v"))
            CommonUtils::printSet(systemPathVariableFilteredSet);
    }

    spdlog::info("Calculating distance Map");
    std::map<std::string, int> distanceMap = WordDistanceHandler::calculateWordDistance(inputCommand, systemPathVariableFilteredSet);

    if (distanceMap.size() == 0) {
        std::cout<<"Could not find any similar commands to \""<<inputCommand<<"\"\n";
        return false;
    } 

    int minValueInMap = INT_MAX;
    for (auto const &entry : distanceMap) {
        if (entry.second < minValueInMap)
            minValueInMap = entry.second;
    }

    std::vector<std::string> similarCommands;
    for (auto const &entry : distanceMap) {
        if (entry.second == minValueInMap) {
            similarCommands.push_back(entry.first);
        }
    }

    if (similarCommands.size() > 1) {
        std::cout<<"Could not find command " + inputCommand + ". Were you looking for these?\n";
        for (auto const &entry : similarCommands)
            std::cout<<"- "<<entry<<"\n";
    } else 
        std::cout<<"Could not find command " + inputCommand + ". Were you looking for \"" + similarCommands[0]<<"\"?\n";

    return true;
}

int main(int argc, char* argv[]) {

    std::string inputCommand;
    
    po::variables_map vm;        
    po::options_description desc("Allowed options");

    try {
        // Handling program parameters
        desc.add_options()
            ("i", po::value<std::string>(), "The input command")
            ("e", "Edit the configuration file")
            ("v", "Verbose mode")
            ("help", "Produce a help message");

        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    

        
        if (!vm.count("v")) {
            spdlog::set_level(spdlog::level::off);
        }

        if (vm.count("i")) {
            inputCommand = vm["i"].as<std::string>();
            spdlog::info("Specified input command: {}", inputCommand);
        }

        if (vm.count("e")) {
            system("editor ~/.smile/settings.json");
            return 0;
        }
        
        if (vm.count("help")) {
            std::cout<<desc<<"\n";
            return 0;
        }

    }
    catch(std::exception& e) {
        std::cerr<<"error: "<<e.what()<<"\n";
        return 1;
    }
    catch(...) {
        std::cerr<<"Exception of unknown type\n";
        return 1;
    }

    Settings settings;

    //TODO: enable database insertion for history if enabled

    //TODO: order results by most similar characters from the start to give more appropriate results?

    suggestCommands(vm, inputCommand, settings);

    return 0;
}
#include <iostream>
#include <map>
#include <set>
#include <cmath>
#include <unordered_set>
#include "../include/Settings.hpp"
#include "../include/WordDistanceHandler.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {

    std::string inputCommand;

    try {
        
        // Handling program parameters
        po::options_description desc("Allowed options");
        desc.add_options()
            ("i", po::value<std::string>(), "The input command")
            ("e", "Edit the configuration file")
            ("help", "Produce a help message");

        po::variables_map vm;        
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    

        

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
    spdlog::info("Settings successufully loaded.");


    // Writing command_not_found_handle funtion in .bashrc file if not present
    int result = system("bash initializer/initialzer.sh");
    if (result != 0)
        spdlog::error("Error executing the initializer script. Return code: {}", result);
    else
        spdlog::info("Initializer script successfully executed.");

    spdlog::info("Printing content of system path vector");
    CommonUtils::printVector(settings.getSystemPathVariablePaths());

    // TODO: write a new program to update the database with each new binaries in the system each time a new bash shell is open
    // to reduce calcultion time 



    std::set<std::string> systemPathVariableSet;
    for (const auto &entry : settings.getSystemPathVariablePaths()) {
        std::cout<<"Content of: "<<entry<<"\n";

        std::vector<std::string> filesInPath = CommonUtils::getListOfFilesInPath(entry, false, true);

        systemPathVariableSet.insert(filesInPath.begin(), filesInPath.end());
    }

    // TODO: use an heuristic to reduce calculation time further
    spdlog::info("Printing content of system path set");
    CommonUtils::printSet(systemPathVariableSet);

    spdlog::info("Before filtering set size: {}", systemPathVariableSet.size());

    std::set<std::string> systemPathVariableFilteredSet;


    std::copy_if(
        systemPathVariableSet.begin(), 
        systemPathVariableSet.end(), 
        std::inserter(systemPathVariableFilteredSet, systemPathVariableFilteredSet.end()), 
        [inputCommand](const std::string& value){
            
            bool lengthCondition = std::abs(static_cast<int>(value.length()) - static_cast<int>(inputCommand.length())) <= 2;

            if (!lengthCondition)
                return lengthCondition;

            std::unordered_set<char> inputCommandCharSet = CommonUtils::getSetOfUniqueCharFromString(inputCommand);
            std::unordered_set<char> currentBinaryCharSet = CommonUtils::getSetOfUniqueCharFromString(value);
            
            std::unordered_set<char> intersectionSet;
            for (char ch : inputCommandCharSet) {
                if (currentBinaryCharSet.find(ch) != currentBinaryCharSet.end()) {
                    intersectionSet.insert(ch);
                }
            }
            
            bool letterCondition = intersectionSet.size() >= (inputCommandCharSet.size() / 2);

            return letterCondition;
        }
    );

    spdlog::info("After filtering set size: {}", systemPathVariableFilteredSet.size());

    spdlog::info("Printing content of system path set after filtering");
    CommonUtils::printSet(systemPathVariableFilteredSet);

    spdlog::info("Calculating distance Map");
    std::map<std::string, int> distanceMap = WordDistanceHandler::calculateWordDistance(inputCommand, systemPathVariableFilteredSet);

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

    std::cout<<"Could not find command " + inputCommand + ". Were you looking for";
    for (auto const &entry : similarCommands) {
        std::cout<<" \""<<entry<<"\"";
    }
    std::cout<<"?\n";



    //spdlog::info("Printing distance Map");
    //WordDistanceHandler::printDistanceMap(distanceMap);
   
    return 0;
}
#include <iostream>
#include <map>
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
            ("help", "Produce a help message")
            ("i", po::value<std::string>(), "The input command");

        po::variables_map vm;        
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    

        if (vm.count("help")) {
            std::cout<<desc<<"\n";
            return 0;
        }

        if (vm.count("i")) {
            inputCommand = vm["i"].as<std::string>();
            spdlog::info("Specified input command: {}", inputCommand);
        } else {
            spdlog::error("Input command was not specified. Exiting...");
            exit(1);
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

    // TODO: use an heuristic to reduce calculation time further

    std::vector<std::string> systemPathVariableList;
    for (const auto &entry : settings.getSystemPathVariablePaths()) {
        std::cout<<"entry: "<<entry<<"\n";
        std::vector<std::string> toInsert = CommonUtils::getListOfFilesInPath(entry, false, true);
        systemPathVariableList.insert(systemPathVariableList.end(), toInsert.begin(), toInsert.end());
        
        /*for (const auto &entry : systemPathVariableList) {
            std::cout<<entry<<"\n";
        }*/
    }
    
    // Testing
    /*
    for (const auto &entry : CommonUtils::getListOfFilesInPath(std::string("/usr/bin"))) {
        std::cout<<entry<<"\n";
    }
    */

    /*std::string word1 = "ciao";
    std::string word2 = "ciTzo";
    std::cout<<"Distance of "<<word1<<" and "<<word2<<": "<<WordDistanceHandler::calculateWordDistance(word1, word2)<<"\n";*/

    //spdlog::info("Printing content of system path vector");
    //CommonUtils::printVector(systemPathVariableList);

    spdlog::info("Calculating distance Map");
    std::map<std::string, int> distanceMap = WordDistanceHandler::calculateWordDistance(inputCommand, systemPathVariableList);

    spdlog::info("Printing distance Map");
    WordDistanceHandler::printDistanceMap(distanceMap);
   
    return 0;
}
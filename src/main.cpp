#include <iostream>
#include "../include/Settings.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {

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
            spdlog::info("Specified input command: {}", vm["i"].as<std::string>());
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


    for (const auto &entry : settings.getSystemPathVariablePaths()) {
        std::cout<<"entry: "<<entry<<"\n";
        std::vector<std::string> systemPathVariableList = CommonUtils::getListOfFilesInPath(entry, false, true);
        
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

    return 0;
}
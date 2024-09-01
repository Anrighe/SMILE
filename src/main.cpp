#include <iostream>
#include "../include/Settings.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {

    try {

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "Produce a help message")
            ("ic", po::value<std::string>(), "The input command");

        po::variables_map vm;        
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    

        if (vm.count("help")) {
            std::cout<<desc<<"\n";
            return 0;
        }

        if (vm.count("ic")) {
            spdlog::info("Specified input command: " + vm["ic"].as<std::string>());
        } else {
            spdlog::error("Input command was not specified.");
        }

    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

    Settings settings;
    
    return 0;
}
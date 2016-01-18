#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>

#include "molecule.h"
#include "constant.h"
#include "runner.h"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nanoARCS.main"));

int main (int argc, char* argv[]) {
    if(argc < 2) {
        return RunnerManager::get()->help(argc, argv);
    }
    RunnerPtr runner = RunnerManager::get()->create(argv[1]);
    if(!runner) {
        return RunnerManager::get()->help(argc, argv);
    }

    const std::string& opt_string = runner->options();
    int opt = -1;
    Properties cmd;
    while((opt = getopt(argc - 1, argv + 1, opt_string.c_str())) != -1) {
        std::string key(1, (char)opt);
        if(optarg == NULL) {
            cmd.put(key, NULL);
        } else {
            cmd.put(key, optarg);
        }
    }
    Arguments arguments;
    for(int i = optind + 1; i < argc; ++i) {
        arguments.push_back(argv[i]);
    }
    const std::string log_config = cmd.get< std::string >("c", kLogConfig);
    if(boost::filesystem::exists(log_config)) {
        log4cxx::PropertyConfigurator::configure(log_config);
    } else {
        log4cxx::BasicConfigurator::configure();
    }

    return runner->run(cmd, arguments);
    std::cout << "hello world" << std::endl;
}

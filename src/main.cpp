#include <iostream>
#include <string>
#include <tuple>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>

#include "molecule.h"
#include "constant.h"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("arcs.main"));

int main (int argc, char* argv[]) {
    std::string help = "Usage: nanoARCS -i inputfile";
    if(argc < 2) {
        std::cout << help << std::endl;
        return 0;
    }
    const std::string opt_string("i:s:h");
    int opt = -1;
    boost::property_tree::ptree cmd;
    while((opt = getopt(argc, argv, opt_string.c_str())) != -1) {
        std::string key(1, (char)opt);
        if(optarg == NULL) {
            cmd.put(key, NULL);
        } else {
            cmd.put(key, optarg);
        }
    }
    if(cmd.find("h") != cmd.not_found()) {
        std::cout << help << std::endl;
        return 0;
    }
    const std::string file = cmd.get< std::string >("i", "NULL");
    size_t scale = cmd.get< size_t >("s", SCALE);
    if( boost::filesystem::exists(file) ) {
        std::ifstream in(file.c_str());
        MoleculeReader reader(in);
        Molecule mol;
        while(reader.read(mol, scale)) {
            std::cout << mol << std::endl;
        }
    } else {
        LOG4CXX_ERROR(logger, boost::format("load %s failed.") % file);
    }
    std::cout << "hello world" << std::endl;
}

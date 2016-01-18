#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <log4cxx/logger.h>

#include "preprocess.h"
#include "molecule.h"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nanoARCS.preprocess"));
int _preprocess_run_(const Properties& opitions, const Arguments& arguments) {
    int r = 0;
    size_t scale = opitions.get< size_t >("s", SCALE);
    FLES::setFLESK(opitions.get< size_t >("K", FLESK) / scale);
    
    FLESIndexTable table;
    const std::string file = opitions.get< std::string >("i", "no_file");
    if( boost::filesystem::exists(file) ) {
        std::ifstream in(file.c_str());
        MoleculeReader reader(in);
        Molecule mol;
        size_t molNum = 0;
        while(reader.read(mol, scale)) {
        //    std::cout << mol << std::endl;
            ++molNum;
            mol.split2FLES(table);
        }   
        LOG4CXX_ERROR(logger, boost::format("read molecule number = [%d]") % molNum);
    } else {
        if(file != "no_file") {
            LOG4CXX_ERROR(logger, boost::format("load %s failed.") % file);
        } else {
            LOG4CXX_ERROR(logger, boost::format("use -i to specify a input file"));
        }
        return 1;
    }
    
    std::cout << "FLES:" << std::endl;
    typedef const std::pair<FLES, std::vector<FLESIndex> > const_pair;
    BOOST_FOREACH(const_pair& x, table) {
        std::cout << x.first << std::endl;
    }
    return r;
}

Preprocess Preprocess::_runner;
Preprocess::Preprocess() : Runner("s:i:hK:") {
    RUNNER_INSTALL("preprocess", this, "preprocess");
}

int Preprocess::run(const Properties& opitions, const Arguments& arguments) {
    int r = 0;
    if( (r = checkOpitions(opitions)) != 0 ) {
        return r;
    }
    return _preprocess_run_(opitions, arguments);
}

int Preprocess::checkOpitions(const Properties& opitions) const {
   if(opitions.find("h") != opitions.not_found()) {
       return printHelp();
   } 
   return 0;
}

int Preprocess::printHelp() const {
    std::cout << "preprocess" << std::endl;
    return 256;
}


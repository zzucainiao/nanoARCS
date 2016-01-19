#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <log4cxx/logger.h>

#include "overlap_graph.h"
#include "category.h"
#include "cluster.h"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nanoARCS.overlap_graph"));
int _overlap_graph_run_(const Properties& opitions, const Arguments& arguments) {
    LOG4CXX_INFO(logger, boost::format("overlap_graph begin"));
    int r = 0;
    const std::string FLESFile = opitions.get< std::string >("i", "no_file");
    FLESIndexTable table;
    if(boost::filesystem::exists(FLESFile)) {
        std::ifstream in(FLESFile.c_str());
        FLESReader reader(in);
        std::pair<FLES, std::vector<FLESIndex> > fles;
        while(reader.read(fles)) {
            std::cout << fles.first << std::endl;
            BOOST_FOREACH(FLESIndex& index, fles.second) {
                std::cout << index._molIndex << " " << index._pos << "|";
            }
            std::cout << std::endl;
            table.insert(fles);
        }
    } else {
        if(FLESFile != "no_file") {
            LOG4CXX_ERROR(logger, boost::format("load %s failed") % FLESFile);
        } else {
            LOG4CXX_ERROR(logger, boost::format("use -i to specify a input file"));
        }
    }
    Cluster cluster;
    Categorys categorys;
    cluster.clusting(table, categorys);
    BOOST_FOREACH(Category& ca, categorys) {
        std::cout << ca << std::endl;
    }
    LOG4CXX_INFO(logger, boost::format("overlap_graph end"));
    return r;
}
int OverlapGraph::run(const Properties& opitions, const Arguments& arguments) {
    int r = 0;
    if((r = checkOpitions(opitions)) != 0) {
        return r;
    }
    r = _overlap_graph_run_(opitions, arguments);
    return r;
}

OverlapGraph OverlapGraph::_runner;
OverlapGraph::OverlapGraph() : Runner("i:") {
    RUNNER_INSTALL("overlap_graph", this, "overlap_graph");
}

int OverlapGraph::checkOpitions(const Properties& opitions) const{
    if(opitions.find("h") != opitions.not_found()) {
        return printHelp();
    } 
    return 0;
}

int OverlapGraph::printHelp() const {
    std::cout << "overlap_graph" << std::endl;
    return 256;
}




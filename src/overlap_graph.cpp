#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/assert.hpp>
#include <log4cxx/logger.h>

#include "overlap_graph.h"
#include "category.h"
#include "cluster.h"
#include "constant.h"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nanoARCS.overlap_graph"));
int _overlap_graph_run_(const Properties& opitions, const Arguments& arguments) {
    LOG4CXX_INFO(logger, boost::format("overlap_graph begin"));
    int r = 0;
    size_t flesK = opitions.get< size_t >("K", FLESK);
    size_t scale = opitions.get< size_t >("s", SCALE);
    FLES::setFLESK(flesK / scale);
    const std::string FLESFile = opitions.get< std::string >("i", "no_file");
    FLESIndexTable table;
    if(boost::filesystem::exists(FLESFile)) {
        std::ifstream in(FLESFile.c_str());
        FLESReader reader(in);
        std::pair<FLES, std::vector<FLESIndex> > fles;
        while(reader.read(fles)) {
            if(fles.first.getSite().size() > FLESLENGTHCUTOFF) { 
                BOOST_ASSERT(fles.second.size() > 0 && fles.second[0]._pos != -1);
                table.insert(fles);
            }
        }
        LOG4CXX_INFO(logger, boost::format("FLES num = %d") % table.size());
    } else {
        if(FLESFile != "no_file") {
            LOG4CXX_ERROR(logger, boost::format("load %s failed") % FLESFile);
        } else {
            LOG4CXX_ERROR(logger, boost::format("use -i to specify a input file"));
        }
    }
    double p_value = 0.02;
    Cluster cluster(scale, p_value);
    Categorys categorys;
    cluster.clusting(table, categorys);
    Graph g(categorys);
    std::string graphFileName = opitions.get< std::string >("o", GRAPHFILE);
    std::ofstream os(graphFileName.c_str());
    os << g;
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




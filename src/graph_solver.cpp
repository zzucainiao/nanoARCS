#include "graph_solver.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <memory>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/assert.hpp>
#include <log4cxx/logger.h>

#include "unique_edge_graph.h"
#include "lp_strategy.h"
#include "max_overlap_strategy.h"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nanoARCS.graph_solver"));
int _graph_solver_run_(const Properties& opitions, const Arguments& arguments) {
    LOG4CXX_INFO(logger, boost::format("graph_solver begin"));
    int r = 0;
    const std::string graphFile = opitions.get< std::string >("i", "no_file");
    const int type = opitions.get< int >("s", 0);
    std::string strategyType("max_overlap");
    if(type == 1) {
        strategyType = "lp";
    }
    if(!boost::filesystem::exists(graphFile)) {
        if(graphFile != "no_file") {
            LOG4CXX_ERROR(logger, boost::format("load %s failed") % graphFile);
        } else {
            LOG4CXX_ERROR(logger, boost::format("can't find graph file. (use -i to specify a input file)"));
            exit(1);
        }
    }

    UniqueEdgeGraph graph;
    graph.read(graphFile);
    std::unique_ptr<Strategy> strategy;
    if(strategyType == "lp") {
        strategy.reset(new LPStrategy(graph));
    } else {
        strategy.reset(new MaxOverlapStrategy(graph));
    }

    strategy->solve();

    LOG4CXX_INFO(logger, boost::format("graph_solver end"));
    return r;
}
int GraphSolver::run(const Properties& opitions, const Arguments& arguments) {
    int r = 0;
    if((r = checkOpitions(opitions)) != 0) {
        return r;
    }
    r = _graph_solver_run_(opitions, arguments);
    return r;
}

GraphSolver GraphSolver::_runner;
GraphSolver::GraphSolver() : Runner("i:s:") {
    RUNNER_INSTALL("graph_solver", this, "graph_solver");
}

int GraphSolver::checkOpitions(const Properties& opitions) const{
    if(opitions.find("h") != opitions.not_found()) {
        return printHelp();
    } 
    return 0;
}

int GraphSolver::printHelp() const {
    std::cout << "graph_solver" << std::endl;
    std::cout << "\t-i OVERLAPGRAPH.file -s [0(max_overlap)|1(lp)]" << std::endl;
    return 256;
}




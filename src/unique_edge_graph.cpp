#include "unique_edge_graph.h"

#include <fstream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assert.hpp>
#include <boost/format.hpp>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nanoARCS.unique_edge_graph"));

int UniqueEdgeGraph::read(const std::string& graphFile) {
    std::ifstream in(graphFile);
    return read(in);
}

int UniqueEdgeGraph::read(std::istream& in) {
    std::string line;
    size_t edgeNum = 0;
    while(std::getline(in, line)) {
        std::vector<std::string> re;
        boost::split(re, line, boost::is_any_of(" \t"));
        size_t a = boost::lexical_cast<size_t>(re[0]);
        size_t b = boost::lexical_cast<size_t>(re[1]);
        BOOST_ASSERT(a < b);
        ++edgeNum;
        int dis = boost::lexical_cast<int>(re[2]);
        int sameNum = boost::lexical_cast<int>(re[3]);
        GraphMap::iterator it = _graph.find(a);
        if(it == _graph.end()) {
            _graph[a] = EdgeList(1, Edge(b, dis, sameNum));
        } else {
            it->second.push_back( Edge(b, dis, sameNum) );
        }

        it = _graph.find(b);
        if(it == _graph.end()) {
            _graph[b] = EdgeList(1, Edge(a, -dis, sameNum));
        } else {
            it->second.push_back( Edge(a, -dis, sameNum) );
        }
    }
    LOG4CXX_INFO(logger, boost::format("edge number = [%d]") % edgeNum);
    return 0;
}

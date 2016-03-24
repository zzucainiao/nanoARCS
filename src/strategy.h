#ifndef _strategy_h
#define _strategy_h

#include "unique_edge_graph.h"

class Strategy {
public:
    Strategy(UniqueEdgeGraph& uniqueEdgeGraph): _uniqueEdgeGraph(uniqueEdgeGraph) {
    } 
    virtual int solve() = 0;
//private:
    UniqueEdgeGraph& _uniqueEdgeGraph;
};

#endif // _strategy_h

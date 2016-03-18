#ifndef _strategy_h
#define _strategy_h

#include "graph.h"

class Strategy {
public:
    Strategy(Graph& graph): _graph(graph) {
    } 
    virtual int solve() = 0;
private:
    Graph& _graph;
};

#endif // _strategy_h

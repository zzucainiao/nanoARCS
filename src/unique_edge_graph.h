#ifndef _unique_edge_graph
#define _unique_edge_graph

#include <iostream>
#include <string>
#include <list>
#include <map>

struct Edge {
    size_t _adj;
    int _dis, _sameNum;
    Edge(size_t adj = -1, int dis = 0, int sameNum = 0): _adj(adj), _dis(dis), _sameNum(sameNum) {
    }
};

class MaxOverlapStrategy;
class LPStrategy;

class UniqueEdgeGraph {
public:
    UniqueEdgeGraph(){} ;
    int read(const std::string& graphfile);
    int read(std::istream& in);
private:
    friend class MaxOverlapStrategy;
    friend class LPStrategy;
    
    using EdgeList = std::list<Edge>;
    using GraphMap = std::map<size_t, EdgeList>;
    GraphMap _graph;
};

#endif //_unique_edge_graph

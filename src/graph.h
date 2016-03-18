#ifndef graph_
#define graph_

#include <iostream>
#include <map>
#include <vector>

#include "category.h"

class Graph {
public:
    Graph(const Categorys& cas) ;
private:
    friend std::ostream& operator<<(std::ostream& os, const Graph& g);
    //map< pair<molIndex_i, molIndex_j>, vector<dis> >
    typedef std::map< std::pair<size_t, size_t>, std::vector<long long> > MolDisMap; 
    MolDisMap _mp;
};

#endif //graph_

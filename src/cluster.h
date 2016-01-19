#ifndef overlap_graph_h_
#define overlap_graph_h_

#include <vector>

#include "category.h"
#include "fles.h"

class Cluster {
public:
    Cluster(){} 
    void clusting(const FLESIndexTable& table, Categorys& categorys);
private:
    void getWindows(size_t sumLength, size_t winNum, std::vector< size_t >& windows) ;
    size_t getType(const std::vector< size_t >& windows, const Category& ca) ;
    void clusting(const Categorys& cas, Categorys& re) ;

};

#endif

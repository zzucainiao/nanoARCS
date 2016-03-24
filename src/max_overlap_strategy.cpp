#include "max_overlap_strategy.h"

#include <iostream>

int MaxOverlapStrategy::solve() {
    std::cout << "max_overlap" << std::endl;
    for(auto& v : _uniqueEdgeGraph._graph) {
        v.second.sort([](const Edge& x, const Edge& y){ 
                            return x._sameNum > y._sameNum;}
                            );
        
    }
    return 0;
}

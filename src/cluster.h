#ifndef cluster_h_
#define cluster_h_

#include <vector>
#include <map>

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
    bool alignment(const Category& x, const Category& y) const ;

};

class Graph {
public:
    Graph(const Categorys& cas) ;
private:
    friend std::ostream& operator<<(std::ostream& os, const Graph& g);
    //map< pair<molIndex_i, molIndex_j>, vector<dis> >
    typedef std::map< std::pair<size_t, size_t>, std::vector<long long> > MolDisMap; 
    MolDisMap _mp;
};

#endif

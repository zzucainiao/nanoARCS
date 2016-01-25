#ifndef cluster_h_
#define cluster_h_

#include <vector>
#include <map>

#include <boost/thread/mutex.hpp>

#include "category.h"
#include "fles.h"

class Cluster {
public:
    Cluster(size_t scale, double p_value = 0.02): _scale(scale), _p_value(p_value){} 
    void clusting(const FLESIndexTable& table, Categorys& categorys);
private:
    void getWindows(size_t sumLength, size_t winNum, std::vector< size_t >& windows) ;
    size_t getType(const std::vector< size_t >& windows, const Category& ca) ;
    void clusting(size_t type, double p_value_threshold, const Categorys& cas, Categorys* re) ;
    double alignment(const Category& x, const Category& y) const ;
    double pMiss(int missNum) const ;
    double score(long long matchError, int missNum) const ;
    double gaussion(long long matchErroe) const ;
    double backGaussion(long long matchError) const ;
    size_t _scale;
    double _p_value;
    boost::mutex _mtx;
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

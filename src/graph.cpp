#include "graph.h"

#include <vector>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/format.hpp>

Graph::Graph(const Categorys& cas) {
    BOOST_FOREACH(const Category& ca, cas) {
        const std::vector< FLESIndex >& indexes = ca.getFLESIndexes();
        for(size_t i = 0; i < indexes.size(); ++i) {
            for(size_t j = i+1; j < indexes.size(); ++j) {
                size_t molIndex_i = indexes[i]._molIndex;
                size_t molIndex_j = indexes[j]._molIndex;
                long long pos_i = indexes[i]._pos;
                long long pos_j = indexes[j]._pos;
                if(molIndex_i == molIndex_j) {
                    continue;
                }
                if(molIndex_i > molIndex_j) {
                    std::swap(molIndex_i, molIndex_j);
                    std::swap(pos_i, pos_j);
                }
                MolDisMap::iterator it = _mp.find( std::make_pair(molIndex_i, molIndex_j) );
                if(it != _mp.end()) {
                    it->second.push_back( pos_i - pos_j );
                } else {
                    _mp.insert( std::make_pair( std::make_pair(molIndex_i, molIndex_j), std::vector<long long>(1, pos_i - pos_j)) );
                }
            }
        }
    }
    typedef std::pair< const std::pair<size_t, size_t>, std::vector<long long> > ValueType; 
    BOOST_FOREACH(ValueType& v, _mp) {
        std::sort(v.second.begin(), v.second.end());
    } 
}

std::ostream& operator<<(std::ostream& os, const Graph& g) {
    typedef std::pair< std::pair<size_t, size_t>, std::vector<long long> > ValueType; 
    BOOST_FOREACH(const ValueType& v, g._mp) {
        //mid or avrage ???
        os << v.first.first << "\t" << v.first.second << "\t" << v.second[v.second.size()/2] << "\t" << v.second.size() << std::endl;
    } 
    return os;
}


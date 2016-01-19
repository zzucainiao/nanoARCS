#include <algorithm>
#include <map>

#include <boost/foreach.hpp>

#include "cluster.h"
#include "constant.h"

void Cluster::clusting(const FLESIndexTable& table, Categorys& categorys) {
    categorys.resize( table.size() );
    typedef const std::pair< FLES, std::vector<FLESIndex> > ValueType;
    size_t i = 0;
    BOOST_FOREACH(ValueType& v, table) {
        categorys[i++].set(v.first.getSite(), v.second);
    }
    size_t maxWinNum = 10;
    int iter = 0;
    while(iter < ITERTIME) {
        size_t sumLength = FLES::getFLESK();
        size_t winNum = std::max((size_t)6, maxWinNum--);
        std::vector< size_t > windows;
        getWindows(sumLength, winNum, windows);
        typedef std::map<size_t, Categorys> Type2Categorys;
        Type2Categorys type2Category;
        BOOST_FOREACH(Category& ca, categorys) {
            size_t type = getType(windows, ca);
            Type2Categorys::iterator it = type2Category.find( type );
            if(it != type2Category.end()) {
                it->second.push_back(ca);
            } else {
                Categorys x;
                x.push_back(ca);
                type2Category.insert( make_pair(type, x) );
            }
        }
        Categorys tmp;
        typedef const std::pair<size_t, Categorys> Value;
        BOOST_FOREACH(Value& v, type2Category) {
            clusting(v.second, tmp);
        }
        categorys.swap(tmp);
    }
}

void Cluster::getWindows(size_t sumLength, size_t winNum, std::vector< size_t >& windows) {

}

size_t Cluster::getType(const std::vector< size_t >& windows, const Category& ca) {
    return 0;
}

void Cluster::clusting(const Categorys& cas, Categorys& re) {

}

#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <map>
#include <queue>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <log4cxx/logger.h>

#include "cluster.h"
#include "constant.h"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nanoARCS.cluster"));
void Cluster::clusting(const FLESIndexTable& table, Categorys& categorys) {
    categorys.resize( table.size() );
    typedef const std::pair< FLES, std::vector<FLESIndex> > ValueType;
    size_t i = 0;
    BOOST_FOREACH(ValueType& v, table) {
        categorys[i++].set(v.first.getSite(), v.second);
    }
    size_t maxWinNum = 10;
    int iter = 0;
    std::srand(time(NULL));
    while(iter++ < ITERTIME) {
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
//            LOG4CXX_DEBUG(logger, boost::format("iterator = %d type = %d size = %d") % iter % v.first % v.second.size());
            clusting(v.second, tmp);
        }
        categorys.swap(tmp);
    }
}

void Cluster::getWindows(size_t sumLength, size_t winNum, std::vector< size_t >& windows) {
    windows.push_back(0);
    size_t lengthPerWin = sumLength / winNum;
    for(size_t i = 0; i < winNum; ++i) {
        windows.push_back(i*lengthPerWin + std::rand() % lengthPerWin);
    }
}

size_t Cluster::getType(const std::vector< size_t >& windows, const Category& ca) {
    size_t re = 0, sum = 0;
    for(size_t i = 0, j = 0; j < windows.size() && i < ca.size(); ++j) {
        while(i < ca.size() && sum + ca[i] < windows[j]) ++i, sum += ca[i];
        if(i != ca.size() && (j+1 == windows.size() || sum + ca[i] < windows[j+1])) {
            re |= (1LL << j); 
        }
    }
    return re;
}

void Cluster::clusting(const Categorys& cas, Categorys& re) {
    std::vector< std::vector<size_t> > adj;
    adj.resize( cas.size() );
    for(size_t i = 0; i < cas.size(); ++i) {
        for(size_t j = i+1; j < cas.size(); ++j) {
            if( alignment(cas[i], cas[j])) {
                adj[i].push_back(j);
                adj[j].push_back(i);
            }
        }
    } 
    std::vector<int> visit(cas.size(), 0);
    for(size_t i = 0; i < cas.size(); ++i) {
        if(visit[i])
            continue;
        Category ca;
        size_t maxDegree = adj[i].size(), center = i;
        std::queue<size_t> Q;
        Q.push(i);
        while(!Q.empty()) {
            size_t now = Q.front();
            ca += cas[now];
            if(adj[now].size() > maxDegree) {
                maxDegree = adj[now].size();
                center = now;
            }
            Q.pop();
            for(size_t j = 0; j < adj[now].size(); ++j) {
                if(!visit[ adj[now][j] ]) {
                    Q.push(adj[now][j]);
                    visit[ adj[now][j] ] = 1;
                }
            }
        }
        ca.setSite(cas[center].getSite());
        re.push_back(ca);
    }
}

bool Cluster::alignment(const Category& x, const Category& y) const {
    std::vector< std::vector<double> > dp(x.size(), std::vector<double>(y.size(), 0.0));
    for(size_t i = 0; i < x.size(); ++i) {
        for(size_t j = 0; j < y.size(); ++j) {
            ;
        }
    }
    return true;
}

Graph::Graph(const Categorys& cas) {
    BOOST_FOREACH(const Category& ca, cas) {
        std::vector< FLESIndex > indexes = ca.getFLESIndexes();
        for(size_t i = 0; i < indexes.size(); ++i) {
            for(size_t j = i+1; j < indexes.size(); ++j) {
                size_t molIndex_i = indexes[i]._molIndex;
                size_t molIndex_j = indexes[j]._molIndex;
                size_t pos_i = indexes[i]._pos;
                size_t pos_j = indexes[j]._pos;
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
        os << v.first.first << "\t" << v.first.second << "\t" << v.second[v.second.size()/2] << "\t" << v.second.size() << std::endl;
    } 
    return os;
}


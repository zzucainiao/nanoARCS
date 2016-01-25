#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>
#include <cfloat>

#include <algorithm>
#include <map>
#include <queue>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include "threadpool-0_2_5-src/threadpool/boost/threadpool.hpp"
#include <log4cxx/logger.h>

#include "cluster.h"
#include "constant.h"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nanoARCS.cluster"));

struct Statistic {
    std::pair<double, double> operator()(std::pair<double, double> x, double y) const {
        return std::pair<double, double>( x.first+y, x.second+y*y );
    }
};
void Cluster::clusting(const FLESIndexTable& table, Categorys& categorys) {
    categorys.resize( table.size() );
    typedef const std::pair< FLES, std::vector<FLESIndex> > ValueType;
    size_t i = 0;
    BOOST_FOREACH(ValueType& v, table) {
        categorys[i++].set(v.first.getSite(), v.second);
    }
    //p-value for score;
    std::vector<double> scores;
    int simpleNumber = 1000;
    for(int i=0; i<simpleNumber; ++i) {
        size_t x = rand() % table.size();
        size_t y = rand() % table.size();
        if(x != y) {
            scores.push_back( alignment(categorys[x], categorys[y]));
        }
    }
    std::pair<double, double> x(0.0, 0.0);
    std::pair<double, double> re = std::accumulate(scores.begin(), scores.end(), x, Statistic()); 

    LOG4CXX_INFO(logger, boost::format("p-value sroce.size = %d mean = %lf var = %lf") % scores.size() % (re.first/scores.size()) % (re.second - (x.first*x.first)/scores.size()) );
    
    boost::math::normal_distribution<> normal(re.first / scores.size(), std::sqrt( re.second - (x.first*x.first / scores.size()) ));
    double p_value_threshold = boost::math::quantile(normal, 1-_p_value);

    LOG4CXX_INFO(logger, boost::format("p-value_threshold = %lf") % p_value_threshold );
    for(i = 0; (1LL<<i)*256 < categorys.size(); ++i) ;
    long long maxWinNum = i-1;
    LOG4CXX_INFO(logger, boost::format("maxWinNum = %d") % maxWinNum);
    int iter = 0;
    //std::srand(time(NULL));
    boost::threadpool::pool pl(64);
    while(iter++ < ITERTIME) {
        LOG4CXX_INFO(logger, boost::format("cluster iter = %d") % iter);
        size_t sumLength = FLES::getFLESK();
        size_t winNum = static_cast<size_t>(std::max(6LL, maxWinNum--));
        //cluster roughly based on windows
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
        int typeNum = 0;
        BOOST_FOREACH(Value& v, type2Category) {
            pl.schedule(boost::bind((&Cluster::clusting), this, v.first, p_value_threshold, v.second, &tmp));
            ++typeNum;
            //clusting(v.first, p_value_threshold, v.second, &tmp);
        }
        size_t active = pl.active();
        size_t pending = pl.pending();
        size_t size = pl.size();
        LOG4CXX_DEBUG(logger, boost::format("threadpool active = %d pending = %d size = %d typeNum = %d") % active % pending % size % typeNum);

        pl.wait();
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

void Cluster::clusting(size_t type, double p_value_threshold, const Categorys& cas, Categorys* re) {
//     LOG4CXX_DEBUG(logger, boost::format("type = %d size = %d") % type % cas.size());
    std::vector< std::vector<size_t> > adj;
    adj.resize( cas.size() );
    for(size_t i = 0; i < cas.size(); ++i) {
        for(size_t j = i+1; j < cas.size(); ++j) {
            if( alignment(cas[i], cas[j]) > p_value_threshold) {
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
        boost::unique_lock< boost::mutex > lock(_mtx);
        re->push_back(ca);
    }
}

double Cluster::alignment(const Category& x, const Category& y) const {
    std::vector< std::vector<double> > dp(x.size() + 1, std::vector<double>(y.size() + 1, DBL_MIN));
    dp[0][0] = 0.0;
    for(int i = 1; i <= x.size(); ++i) {
        dp[i][0] = pMiss(i);
    }
    for(int j = 1; j <= y.size(); ++j) {
        dp[0][j] = pMiss(j);
    }
    for(int i = 1; i <= x.size(); ++i) {
        for(int j = 1; j <= y.size(); ++j) {
            long long lenx = x[i - 1], leny = y[j - 1];
            double mxScore = DBL_MIN;
            for(int miss = 0; miss < 3 && i-1-miss>=0; ++miss) {
                double tmp = dp[i-1-miss][j-1] + score(std::abs(lenx-leny), miss);
                if( i-1-miss - 1 >=0 ) {
                    lenx += x[i-1-miss - 1];
                }
                mxScore = std::max(tmp, mxScore);
            }
            lenx = x[i - 1], leny = y[j - 1];
            for(int miss = 0; miss < 3 && j-1-miss>=0; ++miss) {
                double tmp = dp[i-1][j-1-miss] + score(abs(lenx-leny), miss);
                if( j-1-miss - 1 >= 0) { 
                    leny += y[j-1-miss - 1];
                }
                mxScore = std::max(tmp, mxScore);
            }
            dp[i][j] = mxScore;
        }
    }
    double re = DBL_MIN;
    for(int missx = 0; missx <= 2 && x.size()>=missx; ++missx) {
        for(int missy = 0; missy <= 2 && y.size()>=missy; ++missy) {
            re = std::max(re, dp[ x.size()-missx ][ y.size()-missy ]+pMiss(missx + missy));
        }
    }
    return re;
}

//match intever miss site fit exponential distribution. 
double Cluster::pMiss(int missNum) const {
    double lambda = 1.064;
    return std::log(lambda) + ( -missNum * lambda); 
}

double Cluster::score(long long matchError, int missNum) const {
    return gaussion(matchError) - backGaussion(matchError) + pMiss(missNum);   
}

double Cluster::gaussion(long long matchError) const {
    double mu = 46.0 / _scale;
    double sigma = 570.0 / _scale;
    return 0.0 - 0.5*std::log(2*3.14) - std::log(sigma) - (matchError-mu)*(matchError-mu)/(2*sigma*sigma);
}

double Cluster::backGaussion(long long matchError) const {
    double mu = 1870.0 / _scale;
    double sigma = 10840.0 / _scale;
    return 0.0 - 0.5*std::log(2*3.14) - std::log(sigma) - (matchError-mu)*(matchError-mu)/(2*sigma*sigma);
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


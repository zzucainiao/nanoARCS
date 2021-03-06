#include "cluster.h"

#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cstdio>

#include <algorithm>
#include <map>
#include <queue>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/threadpool.hpp>
#include <log4cxx/logger.h>

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
    std::ofstream os("sameFLES.out");
    //p-value for score;
    std::vector<double> scores;
    int simpleNumber = 10000;
    for(int i=0; i<simpleNumber; ++i) {
        size_t x = rand() % table.size();
        size_t y = rand() % table.size();
        if(x != y) {
            double score = alignment(categorys[x], categorys[y]);
            //getchar();
            if(score < -1000) {
                continue;
            }
            scores.push_back(score);
            if(alignment( categorys[x], categorys[y] ) > 15.380135) {
                os << categorys[x] << std::endl;
                os << categorys[y] << std::endl;
                os << std::endl;
            }
        }
    }

    std::pair<double, double> x(0.0, 0.0);
    std::pair<double, double> re = std::accumulate(scores.begin(), scores.end(), x, Statistic());
    std::ofstream sos("score.out");
    BOOST_FOREACH(double s, scores) {
        sos << s << std::endl;
    }

    std::cout << re.first << " " <<  scores.size() << " " << std::sqrt( re.second/scores.size() - (re.first*re.first)/(scores.size()*scores.size())) << std::endl;
    boost::math::normal_distribution<> normal(re.first / scores.size(), std::sqrt( re.second/scores.size() - (re.first*re.first)/(scores.size()*scores.size())) );
    double p_value_threshold = boost::math::quantile(normal, 1-_p_value);

    int count = std::count_if(scores.begin(), scores.end(), std::bind2nd(std::greater<int>(), p_value_threshold));

    LOG4CXX_INFO(logger, boost::format("sroce.size = %d >p_threshold size = %d mean = %lf var = %lf") % scores.size() % count % (re.first/scores.size()) % (re.second/scores.size() - (re.first*re.first)/(scores.size()*scores.size())) );
    LOG4CXX_INFO(logger, boost::format("p-value = %lf p-value_threshold = %lf") % _p_value % p_value_threshold );
    int iter = 0;
    //std::srand(time(NULL));
    boost::threadpool::pool pl(64);
    while(iter++ < ITERTIME) {
        LOG4CXX_INFO(logger, boost::format("cluster iter = %d Categorys size = %d") % iter % categorys.size());
        size_t sumLength = FLES::getFLESK();
        for(i = 0; (1LL<<i)*256 < categorys.size(); ++i) ;
        size_t winNum = i;
        LOG4CXX_INFO(logger, boost::format("winNum = %d") % winNum);
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
    std::vector< std::vector<double> > dp(x.size() + 1, std::vector<double>(y.size() + 1, -100000));
    dp[0][0] = 0.0;
    for(int i = 1; i <= 2; ++i) {
        dp[i][0] = pMiss(i);
    }
    for(int j = 1; j <= 2; ++j) {
        dp[0][j] = pMiss(j);
    }
    for(int i = 1; i <= x.size(); ++i) {
        for(int j = 1; j <= y.size(); ++j) {
            long long lenx = x[i - 1], leny = y[j - 1];
            double mxScore = -100000;
            for(int miss = 0; miss < 2 && i-1-miss>=0; ++miss) {
                //std::cout << lenx << " " << leny << " " << miss << " " << score(std::abs(lenx-leny), miss) << std::endl;
                double tmp = dp[i-1-miss][j-1] + score(std::abs(lenx-leny), miss);
                if( i-1-miss - 1 >=0 ) {
                    lenx += x[i-1-miss - 1];
                }
                mxScore = std::max(tmp, mxScore);
            }
            lenx = x[i - 1], leny = y[j - 1];
            for(int miss = 0; miss < 2 && j-1-miss>=0; ++miss) {
                //std::cout << lenx << " " << leny << " " << miss << " " << score(std::abs(lenx-leny), miss) << std::endl;
                double tmp = dp[i-1][j-1-miss] + score(std::abs(lenx-leny), miss);
                if( j-1-miss - 1 >= 0) { 
                    leny += y[j-1-miss - 1];
                }
                mxScore = std::max(tmp, mxScore);
            }
            dp[i][j] = mxScore;
        }
    }
    /* 
    for(int i = 0; i <= x.size(); ++i) {
        for(int j = 0; j <= y.size(); ++j) {
            std::cout << std::setw(10) <<dp[i][j];
        }
        std::cout << std::endl;
    }
    */
    double re = -100000;
    for(int missx = 0; missx <= 2 && x.size()>=missx; ++missx) {
        for(int missy = 0; missy <= 2 && y.size()>=missy; ++missy) {
            re = std::max(re, dp[ x.size()-missx ][ y.size()-missy ]+pMiss(missx + missy));
        }
    }
    return re;
}

//match intever miss site fit exponential distribution. 
double Cluster::pMiss(int missNum) const {
    double lambda = 5.0;
    return std::log(lambda) + ((-missNum) * lambda); 
}

double Cluster::score(long long matchError, int missNum) const {
    //return gaussion(matchError) - backGaussion(matchError);   
    return gaussion(matchError) - backGaussion(matchError) + pMiss(missNum);   
}
/*
double Cluster::gaussion(long long matchError) const {
    //double mu = 46.0 / _scale;
    double mu = 0.0 / _scale;
    double sigma = std::sqrt(2.0) * 570.0 / std::sqrt(_scale);
    return 0.0 - 0.5*std::log(2*3.14) - std::log(sigma) - (matchError-mu)*(matchError-mu)/(2*sigma*sigma);
}

double Cluster::backGaussion(long long matchError) const {
    double mu = 1870.0 / _scale;
    double sigma = 10840.0 / std::sqrt(_scale);
    return 0.0 - 0.5*std::log(2*3.14) - std::log(sigma) - (matchError-mu)*(matchError-mu)/(2*sigma*sigma);
}
*/
double Cluster::gaussion(long long delta) const {
    double mu = 0;
    double sigma = 570.0 / _scale * std::sqrt(2.0);
    boost::math::normal_distribution<> n(mu, sigma);
    int interval_left = delta;
    int interval_right = delta + 1;
    if (delta < 0) {
        interval_left = delta - 1;
        interval_right = delta;
    }
    return log(boost::math::cdf(n, interval_right) - boost::math::cdf(n, interval_left));
}
double Cluster::backGaussion(long long delta) const {
    double mu = 0;
    double sigma = 10840.0 / _scale * std::sqrt(2.0);
    boost::math::normal_distribution<> n(mu, sigma);
    int interval_left = delta;
    int interval_right = delta + 1;
    if (delta < 0) {
        interval_left = delta - 1;
        interval_right = delta;
    }
    return log(boost::math::cdf(n, interval_right) - boost::math::cdf(n, interval_left));
}

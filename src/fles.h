#ifndef fles_h_
#define fles_h_

#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "constant.h"

class FLES {
public:
    typedef std::vector< size_t > Site;
    FLES(){
    }
    FLES(const Site& site): _site(site){
    }
    bool operator<(const FLES& x) const {
        return lexicographical_compare(_site.begin(), _site.end(), x._site.begin(), x._site.end());
    }
    bool operator==(const FLES& x) const {
        return !(*this < x) && !(x < *this);
    }
    struct cal {
        size_t operator()(size_t x, size_t y) const {
            return x*10 + y;
        }
    };
    size_t hash() const{
        static std::hash< size_t > hasher;
        return hasher( std::accumulate(_site.begin(), _site.end(), 0, cal()) );
    }
    const Site& getSite() const{
        return _site;
    }
    void setSite(const Site site) {
        _site = site;
    }
    void addSite(size_t s) {
        _site.push_back(s);
    }
    void clear() {
        _site.clear();
    }
    static void setFLESK(size_t FLESK) {
        _FLESK = FLESK;
    }
    static size_t getFLESK() {
        return _FLESK;
    }
private:
    friend std::ostream& operator<<(std::ostream& os, const FLES& x) ;
    static size_t _FLESK;
    Site _site;
};

struct FLESHasher {
    size_t operator()(const FLES& x) const {
        return x.hash();
    }
};
struct FLESIndex {
    size_t _molIndex;
    long long _pos; 
    FLESIndex(size_t molIndex = -1, long long pos = -1): _molIndex(molIndex), _pos(pos) {}
};

class FLESReader {
public:
    FLESReader(std::istream& in): _in(in) {
    }
    bool read(std::pair<FLES, std::vector<FLESIndex> >& fles);
private:
    std::istream& _in;
};
template<class V>
using FLESTable = std::unordered_map< FLES, V, FLESHasher >;

typedef FLESTable< std::vector<FLESIndex> > FLESIndexTable;
#endif // fles_h_


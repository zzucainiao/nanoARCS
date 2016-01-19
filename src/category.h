#ifndef category_h_
#define category_h_

#include <vector>

#include "fles.h"

class Category {
public:
    Category(){}
    Category(const std::vector< FLESIndex >& FLESIndexes, const FLES::Site& site): _FLESIndexes(FLESIndexes), _centerSite(site) {}
    void set(const FLES::Site& site, const std::vector< FLESIndex >& FLESIndexes) ;
private:
    std::vector< FLESIndex > _FLESIndexes;
    FLES::Site _centerSite;
};

typedef std::vector< Category > Categorys;
#endif

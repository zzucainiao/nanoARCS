#ifndef category_h_
#define category_h_

#include <vector>

#include "fles.h"

class Category {
public:
    Category(){}
    Category(const std::vector< FLESIndex >& FLESIndexes, const FLES::Site& site): _FLESIndexes(FLESIndexes), _centerSite(site) {}
    void set(const FLES::Site& site, const std::vector< FLESIndex >& FLESIndexes) ;
    void setSite(const FLES::Site& site) ;
    const FLES::Site& getSite() const;
    const std::vector< FLESIndex >& getFLESIndexes() const ;
    size_t size() const;
    const size_t& operator[](int i) const;
    Category& operator+=(const Category& x) ;
private:
    friend std::ostream& operator<<(std::ostream& os, const Category& ca);
    std::vector< FLESIndex > _FLESIndexes;
    FLES::Site _centerSite;
};

typedef std::vector< Category > Categorys;
#endif

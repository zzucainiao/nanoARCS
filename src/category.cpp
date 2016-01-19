#include "category.h"

void Category::set(const FLES::Site& site, const std::vector< FLESIndex >& FLESIndexes) {
    _FLESIndexes = FLESIndexes;
    _centerSite = site;
}

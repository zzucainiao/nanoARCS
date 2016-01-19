#include <iostream>

#include "category.h"

void Category::set(const FLES::Site& site, const std::vector< FLESIndex >& FLESIndexes) {
    _FLESIndexes = FLESIndexes;
    _centerSite = site;
}

std::ostream& operator<<(std::ostream& os, const Category& ca) {
    for(FLES::Site::const_iterator it = ca._centerSite.begin(); it != ca._centerSite.end(); ++it) {
        os << *it << " ";
    }
    os << std::endl;
    for(std::vector<FLESIndex>::const_iterator it = ca._FLESIndexes.begin(); it != ca._FLESIndexes.end(); ++it) {
        os << "[" << it->_molIndex << " " << it->_pos << "]";
    }
    return os;
}

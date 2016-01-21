#include <iostream>

#include <boost/foreach.hpp>

#include "category.h"

void Category::set(const FLES::Site& site, const std::vector< FLESIndex >& FLESIndexes) {
    _FLESIndexes = FLESIndexes;
    _centerSite = site;
}

void Category::setSite(const FLES::Site& site) {
    _centerSite = site;
}

const FLES::Site& Category::getSite() const {
    return _centerSite;
}

const std::vector< FLESIndex >& Category::getFLESIndexes() const {
    return _FLESIndexes;
}

size_t Category::size() const {
    return _centerSite.size();
}

const size_t& Category::operator[](int i) const {
    return _centerSite[i];
}

Category& Category::operator+=(const Category& x) {
    BOOST_FOREACH(const FLESIndex& ind, x._FLESIndexes) {
        _FLESIndexes.push_back(ind);
    }
    return *this;
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

#include <iostream>

#include "fles.h"

size_t FLES::_FLESK = 100000;

std::ostream& operator<<(std::ostream& os, const FLES& x) {
    for(FLES::Site::const_iterator it = x._site.begin(); it != x._site.end(); ++it) {
        os << *it << " ";
    }
    return os;
}

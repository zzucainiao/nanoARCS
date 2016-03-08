#include "fles.h"

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nonaARCS.fles"));

size_t FLES::_FLESK = 100000;
std::ostream& operator<<(std::ostream& os, const FLES& x) {
    for(FLES::Site::const_iterator it = x._site.begin(); it != x._site.end(); ++it) {
        os << *it << " ";
    }
    return os;
}

bool FLESReader::read(std::pair<FLES, std::vector<FLESIndex> >& fles) {
    enum TYPE{
        SITE,
        POS
    };
    if(_in) {
        fles.first.clear();
        fles.second.clear();
        std::string tmp;
        TYPE type = SITE;
        while(std::getline(_in, tmp)) {
            if(tmp.length() == 0 || tmp[0] == '#') {
                continue;
            }
            if(type == SITE) {
                std::vector< std::string > substrs;
                boost::split(substrs, tmp, boost::is_any_of(" \t\n\r"));
                BOOST_FOREACH(std::string& str, substrs) {
                    boost::trim(str);
                    if(str.length() != 0) {
                        fles.first.addSite( boost::lexical_cast< size_t >(str) ); 
                    }
                }
                type = POS;
            } else if(type == POS){
                std::vector< std::string > substrs;
                boost::split(substrs, tmp, boost::is_any_of("|"));
                BOOST_FOREACH(std::string& str, substrs) {
                    boost::trim(str);
                    if(str.length() != 0) {
                        size_t molIndex;
                        long long pos;
                        sscanf(str.c_str(), "%lu%lld", &molIndex, &pos);
                        LOG4CXX_TRACE(logger, boost::format("molIndex = %d pos = %d") % molIndex % pos);
                        fles.second.push_back( FLESIndex(molIndex, pos) );
                    }
                }
                return true;
            }
        }
        return false;
    }
    return false;
}


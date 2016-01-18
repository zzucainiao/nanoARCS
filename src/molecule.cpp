#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "molecule.h"

std::ostream& operator<<(std::ostream& os, const Molecule& mol) {
    os << mol._index << std::endl;
    for(Molecule::Site::const_iterator it = mol._site.begin(); it != mol._site.end(); ++it) {
        os << (*it) << " ";
    }
    os << std::endl;
    for(Molecule::Quality::const_iterator it = mol._quality.begin(); it != mol._quality.end(); ++it) {
        os << (*it) << " ";
    }
    return os;
}

int Molecule::split2FLES(FLESIndexTable& table) const {
    size_t k = FLES::getFLESK(), len = 0;
    Site s;
    for(Site::const_iterator it = _site.begin(); it != _site.end(); ++it) {
        if(len + *it > 1.1*k) {
            FLES x(s);
            FLESIndexTable::iterator jt = table.find(x);
            if(jt != table.end()) {
                jt->second.push_back(FLESIndex(_index, 0));
            } else {
                table[x] = std::vector<FLESIndex>(1, FLESIndex(_index, 0));
            }
            s.clear();
            s.push_back(*it);
            len = *it;
        } else {
            s.push_back(*it);
            len += *it;
        }
    }
    if(len > 0.8*k) {
        FLES x(s);
        FLESIndexTable::iterator jt = table.find(x);
        if(jt != table.end()) {
            jt->second.push_back(FLESIndex(_index, 0));
        } else {
            table[x] = std::vector<FLESIndex>(1, FLESIndex(_index, 0));
        }
    }
    return 0;
}

bool MoleculeReader::read(Molecule& mol, size_t scale) {
    enum TYPE{
        LINE1, MOL, QX11, QX12
    };
    TYPE type = LINE1;
    std::string tmp;
    if(_in) {
        while(getline(_in, tmp)) {
            if(tmp.length() == 0 || tmp[0] == '#') {
                continue;
            }
            if(type == LINE1) {
                type = MOL;
                std::vector< std::string > line1;
                boost::split(line1, tmp, boost::is_any_of(" \t\n\r"));
                mol._index = boost::lexical_cast< int >( line1[1] ); 
            } else if(type == MOL) {
                std::vector< std::string > stringSites;
                boost::split(stringSites, tmp, boost::is_any_of(" \t\n\r"));
                size_t pre = -1;
                for(std::vector< std::string >::iterator it = stringSites.begin(); it != stringSites.end(); ++it) {
                    boost::trim(*it); 
                    if((*it).length() == 0) {
                        continue;
                    }
                    if(pre == -1) {
                        pre = boost::lexical_cast< double >(*it);
                    } else {
                        size_t now = boost::lexical_cast< double >(*it);
                        if( (now - pre) / scale >= 1) {
                            mol._site.push_back((now - pre) / scale);
                            pre = now;
                        }
                    }
                }
                type = QX11;
            } else if(type == QX11) {
                std::vector< std::string > stringQua;
                boost::split(stringQua, tmp, boost::is_any_of(" \t\n\r"));
                for(std::vector< std::string >::iterator it = stringQua.begin(); it != stringQua.end(); ++it) {
                    if( it == stringQua.begin() ) {
                        continue; //jump QX11
                    } else {
                        boost::trim(*it);
                        if((*it).length() > 0) {
                            mol._quality.push_back( boost::lexical_cast< double >(*it) );
                        }
                    }
                }
                type = QX12;
            } else if(type == QX12) {
                return true;
            }
        }
        return false;
    }
    return false;
}



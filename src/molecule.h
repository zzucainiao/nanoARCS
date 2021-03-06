#ifndef molecule_h_
#define molecule_h_

#include <iostream>
#include <vector>

#include "fles.h"

class Molecule {  
public:
    typedef std::vector<size_t> Site;
    typedef std::vector<double> Quality;
    Molecule() {};
    Molecule(size_t index, const Site& site, const Quality& quality): _index(index), _site(site), _quality(quality) {
    }
    int split2FLES( FLESIndexTable& table ) const ;
    void clear() ;
    size_t size() const {
        return _site.size();
    }
public:
    size_t _index;
    Site _site;
    Quality _quality;
private:
    friend class MoleculeReader;
    friend std::ostream& operator<<(std::ostream& os, const Molecule& mol);
};

class MoleculeReader {
public:
    MoleculeReader(std::istream& in): _in(in) {
    }
    bool read(Molecule& mol, size_t scale) ;
private:
    std::istream& _in;
};

#endif // molecule_h_

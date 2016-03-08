#ifndef preprocess_h_
#define preprocess_h_

#include "runner.h"

class Preprocess : public Runner {
    Preprocess() ;
    virtual int run(const Properties& opitions, const Arguments& arguments) ;
private:
    int checkOpitions(const Properties& opitions) const;
    int printHelp() const;
    static Preprocess _runner;
};

#endif // preprocess_h_ 

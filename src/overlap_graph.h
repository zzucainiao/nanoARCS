#ifndef overlap_graph_h_
#define overlap_graph_h_

#include "runner.h"

class OverlapGraph : public Runner {
    OverlapGraph() ;
    virtual int run(const Properties& opitions, const Arguments& arguments) ;
private:
    int checkOpitions(const Properties& opitions) const;
    int printHelp() const;
    static OverlapGraph _runner;
};

#endif // overlap_graph_h_

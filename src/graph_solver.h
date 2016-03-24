#ifndef graph_solver_h_
#define graph_solver_h_

#include "runner.h"

class GraphSolver : public Runner {
    GraphSolver() ;
    virtual int run(const Properties& opitions, const Arguments& arguments) ;
private:
    int checkOpitions(const Properties& opitions) const;
    int printHelp() const;
    static GraphSolver _runner;
};

#endif // graph_solver_h_

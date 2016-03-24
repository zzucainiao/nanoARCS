#ifndef _max_overlap_strategy_h
#define _max_overlap_strategy_h

#include "strategy.h"

class MaxOverlapStrategy: public Strategy{
public:
    using Strategy::Strategy;
    virtual int solve();
};

#endif // _max_overlap_strategy_h

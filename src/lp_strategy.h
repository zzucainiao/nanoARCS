#ifndef lp_strategy_h
#define lp_strategy_h

#include "strategy.h"

class LPStrategy: public Strategy{
public:
    using Strategy::Strategy;
    virtual int solve();
};

#endif // lp_strategy_h

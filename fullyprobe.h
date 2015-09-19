#ifndef FP_H
#define FP_H

//#include <set>

#include <tuple>
using std::make_tuple;
using std::get;
#include "set.h"
#include "linesolve.h"
#include "cdef.h"
#include "board.h"

class FullyProbe {
    public:
        Board gp[25][25][2];
        Board max_g0 , max_g1;
        int method;

        myset P;
				myset oldP;
};

double choose( int method , int mp1 , int mp0 );
int fp2( FullyProbe& , LineSolve& , Board& );
int probe( FullyProbe& , LineSolve& , Board& , int , int );
int probeG( FullyProbe&  , LineSolve& ,  int , int , uint64_t );
void setBestPixel( FullyProbe& , Board& );

#endif

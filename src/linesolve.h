#ifndef LINESOLVE_H
#define LINESOLVE_H

#include "cdef.h"
#include "board.h"
#include "Hash.h"
#include <stdint.h>

// don't share with other thread when you wanna parallism
class LineSolve {
    public:
				// problem info
				Clue clue[50];

				// additional info 
        int low_bound[50][14];

				Board solvedBoard;

        LineSolve();
        LineSolve(int*);
        void load(int*);
    private:
        void init();
};

int fixBU( LineSolve& , int , const uint64_t& , int , uint64_t& );
int propagate( LineSolve& , Board& );

#endif

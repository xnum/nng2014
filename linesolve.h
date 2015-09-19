#ifndef LINESOLVE_H
#define LINESOLVE_H

#include "cdef.h"
#include "board.h"
#include "Hash.h"
#include <stdint.h>

class LineSolve {
    public:
        uint64_t line ,newLine;
        int lineNum;

				// problem info
				Clue clue[50];

				// additional info 
        int low_bound[50][14];
				int needCalc[50];

				// share with all problems
        uint64_t value0[28];
        uint64_t value1[28];

				Board solvedBoard;

        LineSolve();
        LineSolve(int*);
        void load(int*);
    private:
        void init();
};

int fixBU( LineSolve& , int );
int propagate( LineSolve& , Board& );

#endif

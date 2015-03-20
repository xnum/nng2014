#ifndef LINESOLVE_H
#define LINESOLVE_H

#include "bcache.h"
#include "cdef.h"
#include "board.h"
#include "rbtree.h"
#include <stdint.h>

class LineSolve {
    public:
        uint64_t line ,newLine;
        int lineNum;
        int probN;
        int low_bound[50][14];
        uint8_t fixTable[27][14];
        uint8_t preFixTable[50][27][14];
        uint64_t value0[28];
        uint64_t value1[28];

        int data[50*14];

        Rbtree queryTable;
		BigCache *bc;

        LineSolve();
        LineSolve(int*,int);
        LineSolve(int*,int,BigCache*);
        void load(int*,int);
		void clearCache(){ queryTable.clear(); }
    private:
        void init();
};

int fix( LineSolve& , int , int );
int propagate( LineSolve& , Board& );

#endif

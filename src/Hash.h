#ifndef _HASH_H_
#define _HASH_H_

#include "cdef.h"



#define HTABLE_SIZE 9999991ULL


struct hashNode{
	Clue lineProblem; uint64_t nowString; uint64_t settleString;
};



void initialHash();

void insertHash(const Clue& problem,const uint64_t& nowString,const uint64_t& settleString );

bool findHash(const Clue& problem,const uint64_t& nowString, uint64_t& settleString);

void genHash(Clue& clue);






#endif 

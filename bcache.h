#ifndef BCACHE_H
#define BCACHE_H

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#ifdef ZHASH
#include <czmq.h>
#include "cdef.h"
#endif


#include "cdef.h"

//#define encrypt(a,b,c) (((uint64_t)keyTable[a*50+b])<<50|c)
#define decrypt(a) (a&FILL)

using namespace std;

int canUseBigCache();

class BigCache
{
	public:
		BigCache();
		~BigCache();
		void insert(int,int,uint64_t,uint64_t);
		uint64_t query(int,int,uint64_t);
		void clear();
        static const uint64_t NOT_FOUND = 0xFFFFFFFFFFFFFFFFLL;
        static const uint64_t ANS_ERR = 0LL;

    private:
        uint64_t curr_used;
		map<size_t,size_t> keyTable;

#ifdef ZHASH
		zhash_t *hash;
		uint64_t *pool;
		int pool_ptr;
#else
		map<uint64_t,uint64_t> table;
#endif

		uint64_t encrypt(uint64_t a,uint64_t b,uint64_t c)
		{
			--a;
			uint64_t newv =  (((uint64_t)keyTable[a*50+b])<<50|c);
			//printf("val=%llu\n",keyTable[a*50+b]);
			//printf("a=%llu,b=%llu\n",a,b);
			//printf("%016llx\n%016llx\n\n",c,newv);
			//if( c == newv )
				//exit(123);
			return newv;
		}
};

#endif

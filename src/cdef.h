#ifndef CDEF_H
#define CDEF_H

#include <cstring>
#include <cstdio>
#include <stdint.h>
#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <cmath>

#ifdef __SSE4_2__
	#include <nmmintrin.h>
#endif

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

// program parameter
#define PATTERN_DROP_THRESHOLD 2
#define MAX_PERM		7
#define PROBLEM_START 	1
#define PROBLEM_END 	450
#define DFS_MAX_TIMES 	500000

//file name
#define INPUT_FILE_NAME "input.txt"
#define OUTPUT_FILE_NAME "output.txt"

// choose method
#define CH_SUM 1
#define CH_MIN 2
#define CH_MAX 3
#define CH_MUL 4
#define CH_SQRT 5
#define CH_MIN_LOGM 6
#define CH_MIN_LOGD 7

#define Dual_for(x,y) for(size_t x=0;x<25;++x)for(size_t y=0;y<25;++y)

#define __GET(n,m) ((n>>(m<<1))&0x3LL)
#define __SET(n,m,b) (n=(n&~(0x3LL<<(m<<1)))|(((uint64_t)(b))<<(m<<1)))
// fot test
#define __SE(n,m,b) __SET(n,m,b)
//#define __SE(n,m,b) (n&~(b<<(m<<1))) 

// status
#define CONFLICT -1
#define INCOMP 0
#define SOLVED 1

// status for new method
#define SAME -1
#define REFLECT 1
#define MUTEX 0
#define BOTH_U 3

// for readable
#define FILL ((0x1LL<<50)-0x1LL)
#define MEMSET_ZERO(arr) (memset(arr,0,sizeof(arr)))

// use in fix
#define LS_NANS 0
#define LS_YES 1
#define LS_NO 2

// bit define
#define BIT_ZERO    0x1LL
#define BIT_ONE     0x2LL
#define BIT_UNKNOWN 0x3LL

// to help record in hash
struct Clue {
	uint8_t num[14];
	int count;
	uint64_t hash;
};

#endif

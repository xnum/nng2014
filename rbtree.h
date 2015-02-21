#ifndef RBTREE_H
#define RBTREE_H
#include <stdint.h>
#include <map>
#ifdef ZHASH
#include <czmq.h>
#include "cdef.h"
#endif

class Rbtree {
    public:
        Rbtree();
        ~Rbtree();
        void insert(int,uint64_t ,uint64_t);
        uint64_t query(int,uint64_t);
        void clear();
#ifdef ZHASH
        zhash_t *hash[50];
        uint64_t *pool;
        int pool_ptr;
#else
        std::map<uint64_t,uint64_t> table[50];
#endif
        static const uint64_t NOT_FOUND = 0xFFFFFFFFFFFFFFFFLL;
        static const uint64_t ANS_ERR = 0LL;

    private:
        uint64_t curr_used;
};

#endif

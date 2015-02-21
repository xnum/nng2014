#include "rbtree.h"
#include <cstdio>


Rbtree :: Rbtree() {
    curr_used = 0;
#ifdef ZHASH
    for ( int i = 0 ; i < 50 ; ++i )
    {
        hash[i] = zhash_new();
    }
    pool = (uint64_t*) malloc( sizeof(uint64_t) * 1024 * 1024 * 80 );
    if( pool == NULL )
    {
        puts("malloc error");
        exit(123);
    }
    pool_ptr = 0;
#endif
}

Rbtree :: ~Rbtree() {
#ifdef ZHASH
    for ( int i = 0 ; i < 50 ; ++i )
    {
        zhash_destroy( &hash[i] );
    }
    free(pool);
#endif
}

void Rbtree :: insert(int line , uint64_t key , uint64_t val ) {
    if( curr_used > 600L*1024L*1024L )
        clear();
    curr_used += sizeof(uint64_t) *8;
#ifdef ZHASH
    if( pool_ptr+1 >= 1024 * 1024 * 80 )
        clear();
    pool[pool_ptr] = val;
    zhash_insert( hash[line] , (char*)&key , &pool[pool_ptr] );
    pool_ptr++;
#else
    table[line][key] = val;
#endif
}

uint64_t Rbtree :: query(int line , uint64_t key) {
#ifdef ZHASH
    uint64_t* res = (uint64_t*)zhash_lookup( hash[line] , (char*)&key );
    if( res == NULL )
        return NOT_FOUND;
    else
        return *res;
#else
    std::map<uint64_t,uint64_t>::iterator it = table[line].find(key);
    if( it == table[line].end() )
        return NOT_FOUND;
    else
        return it->second;
#endif
}

void Rbtree :: clear() {
    curr_used = 0;
#ifndef NDEBUG
    puts("CLEAR==============================");
#endif
    for( int i = 0 ; i < 50 ; ++i ) {
#ifdef ZHASH
        zhash_destroy( &hash[i] );	
        hash[i] = zhash_new();
        pool_ptr = 0;
#else
        table[i].clear();
#endif
    }
}

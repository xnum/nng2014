#include "test.h"

#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;


int test :: checkType( Board a , Board b , int x , int y )
{
    if( getBit(a,x,y) == getBit(b,x,y) )
        return SAME;
    else if( getBit(a,x,y) == BIT_ZERO && getBit(b,x,y) == BIT_ONE )
        return REFLECT;
    else if( getBit(a,x,y) == BIT_ONE && getBit(b,x,y) == BIT_ZERO )
        return REFLECT;
    else if( getBit(a,x,y) == BIT_UNKNOWN && getBit(b,x,y) == BIT_UNKNOWN )
        return BOTH_U;
    else
        return MUTEX;
}

int test :: compare( vector<node> &unk , Board a , Board b )
{
    int type = BOTH_U;
    size_t i;
    for( i = 0 ; i < unk.size() ; ++i )
    {
        int t = checkType( a , b , unk[i].x , unk[i].y );
        if( t == BOTH_U )
            continue;
        else
        {
            type = t;
            break;
        }
    }

    if( type == MUTEX )
        return MUTEX;

    // only SAME , REFLECT arrived here

    for( ; i < unk.size() ; ++i )
    {
        int t = checkType( a , b , unk[i].x , unk[i].y );
        if( t != BOTH_U && t != type )
            return MUTEX;
    }
    return type;
}

test :: test( FullyProbe& fp , Board &g )
{
    vector<node> unk;

    for( int i = 0 ; i < 25 ; ++i )
        for( int j = 0 ; j < 25 ; ++j ) 
            if( getBit( g , i , j ) == BIT_UNKNOWN )
            {
                node tmp;
                tmp.x = i;
                tmp.y = j;
                unk.push_back( tmp );
            }

    printf("%zu\n" , unk.size() );

    int count = 0;
    size_t p = 0;
    int c = 0;
    while(1)
    {
        for( ; p < unk.size() ; ++p )
        {
            if( unk[p].x != 100 && unk[p].y != 100 )
                break;
        }
        if( p >= unk.size() )
            break;
        node sel = unk[p];
        Board &gp0 = fp.gp[sel.x][sel.y][0];
        Board &gp1 = fp.gp[sel.x][sel.y][1];

        vector<node> set; // gp0 & gp1
        for( int i = 0 ; i < 25 ; ++i )
            for( int j = 0 ; j < 25 ; ++j )
            {
                if( REFLECT == checkType( gp0 , gp1 , i , j ) )
                {
                    node tmp;
                    tmp.x = i;
                    tmp.y = j;
                    set.push_back(tmp);
                }
            }

        unk[p].x=100;
        unk[p].y=100;
        if( set.size() <= 2 )
        {
            c++;
            continue;
        }
        count++;

        vec.push_back( gp0 );
        vec.push_back( gp1 );

        for( size_t k = 0 ; k < set.size() ; ++k )
        {
            for( size_t i = 0 ; i < unk.size() ; ++i )
            {
                if( set[k].x==unk[i].x && set[k].y == unk[i].y )
                {
                    unk[i].x = 100;
                    unk[i].y = 100;
                }
            }
        }

    }

    count1 = count;
    count2 = c;

}


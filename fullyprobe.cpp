#include "fullyprobe.h"
#include "linesolve.h"
#include "board.h"
#include <cstdio>
#include <algorithm>
using namespace std;

int fp2 ( FullyProbe& fp , LineSolve& ls , Board& board )
{

    int res = propagate( ls , board);
    if( res != INCOMP )
        return res;

    Dual_for(i,j)
        fp.gp[i][j][0] = fp.gp[i][j][1] = board;

    int solvedCount = -1;
    while(1)
    {

        if( board.size == solvedCount )
            break;
        else
            solvedCount = board.size;

        fp.P.clear();

        Dual_for(i,j)
            if( getBit( board,i,j ) == BIT_UNKNOWN )
            {
                fp.P.insert( i*25+j );
                setBit( fp.gp[i][j][0], i,j,BIT_ZERO );
                setBit( fp.gp[i][j][1], i,j,BIT_ONE );
            }
            else
            {
                fp.gp[i][j][0].empty = true;
                fp.gp[i][j][1].empty = true;
            }

        while(1)
        {
            int p = fp.P.begin();
            if( p == -1 )
                break;

            res = probe( fp , ls , board , p/25 , p%25);
            if( res == SOLVED || res == CONFLICT )
                return res;
        }

        getSize(board);
    }

    int theMaxX = 0 , theMaxY = 0 , theMaxVal = 0;
    double theMaxV = 0;

    Dual_for(i,j)
        if( fp.gp[i][j][0].empty == false && fp.gp[i][j][1].empty == false )
        {
            double ch = choose( fp.method , 
                    fp.gp[i][j][1].size-board.size ,
                    fp.gp[i][j][0].size-board.size );
            if(  ch > theMaxV ) 
            {
                theMaxX = i;
                theMaxY = j;
                theMaxVal = fp.gp[i][j][0].size > fp.gp[i][j][1].size ? 0 : 1;
                theMaxV = ch;
            }
        }

    fp.max_g0 = fp.gp[theMaxX][theMaxY][theMaxVal];
    fp.max_g1 = fp.gp[theMaxX][theMaxY][theMaxVal==1?0:1];

    return INCOMP;
}

#define vlog(x) (log(x+1)+1)
double choose( int method , int mp1 , int mp0 )
{
    switch(method)
    {
        case CH_SUM:
            return mp1+mp0;
            break;
        case CH_MIN:
            return min(mp1,mp0);
            break;
        case CH_MAX:
            return max(mp1,mp0);
            break;
        case CH_MUL:
            return ++mp1 * ++mp0;
            break;
        case CH_SQRT:
            return min(mp1,mp0)+sqrt( max(mp1,mp0)/(min(mp1,mp0)+1));
            break;
        case CH_MIN_LOGM:
            return min(mp1,mp0)+vlog(mp1)*vlog(mp0);
            break;
        case CH_MIN_LOGD:
            return min(mp1,mp0)+abs(vlog(mp1)-vlog(mp0));
            break;
        default:
            return ++mp1*++mp0;
    }
}

int probe( FullyProbe& fp , LineSolve& ls , Board &board , int pX ,int pY )
{

    for( int i = 0 ; i < 50 ; ++i )
    {
        fp.gp[pX][pY][0].data[i] &= board.data[i];
        fp.gp[pX][pY][1].data[i] &= board.data[i];
    }

    int p0 = probeG( fp ,ls ,pX ,pY ,BIT_ZERO );
    if( p0 == SOLVED )
        return SOLVED;
    int p1 = probeG( fp ,ls ,pX ,pY ,BIT_ONE );
    if( p1 == SOLVED )
        return SOLVED;

    if( p0==CONFLICT && p1==CONFLICT )
    {
        return CONFLICT;
    }
    else if( p1==CONFLICT )
    {
        board = fp.gp[pX][pY][0];
        fp.gp[pX][pY][1].empty = true;
        fp.gp[pX][pY][0].empty = true;
    }
    else if( p0==CONFLICT )
    {
        board = fp.gp[pX][pY][1];
        fp.gp[pX][pY][0].empty = true;
        fp.gp[pX][pY][1].empty = true;
    }
    else
    {
        for ( int i = 0 ; i < 50 ; ++i )
            board.data[i] = fp.gp[pX][pY][0].data[i] | fp.gp[pX][pY][1].data[i];
    }
    return INCOMP;
}

int probeG( FullyProbe& fp ,LineSolve& ls ,int pX ,int pY ,uint64_t pVal )
{
    pVal -= BIT_ZERO;
    Board newG = fp.gp[pX][pY][pVal];
    int newGstate = propagate(ls , newG );
    if( newGstate == SOLVED )
        return SOLVED;

#ifdef USE_FP2
    for( int _x = 0 ; _x < 25 ; ++_x )
    {
        uint64_t tmp = newG.data[_x] ^ fp.gp[pX][pY][pVal].data[_x];
        if( !tmp )
            continue;

        int pos = 0;

        while( 0 != (pos=__builtin_ffsll(tmp)) )
        {
            pos--;
            tmp &= tmp-1;
            int _y = pos>>1,
                _v = pos&1;

            if(_x!=pX&&_y!=pY)
            {
                fp.P.insert( _x*25 + _y );
                for( int i = 0 ; i < 50 ; ++i )
                    fp.gp[_x][_y][_v].data[i] &= fp.gp[pX][pY][pVal==0L?1:0].data[i];	
            }
        }
    }
#endif

    fp.gp[pX][pY][pVal] = newG;

    return newGstate;
}

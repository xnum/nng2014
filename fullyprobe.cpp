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

	/*
	   int solvedCount = -1;
	   while(1)
	   {

	   if( board.size == solvedCount )
	   break;
	   else
	   solvedCount = board.size;
	 */

	Dual_for(i,j)
		if( getBit( board,i,j ) == BIT_UNKNOWN )
		{
			fp.P.insert( i*25+j );
			setBit( fp.gp[i][j][0], i,j,BIT_ZERO );
			setBit( fp.gp[i][j][1], i,j,BIT_ONE );
		}

	while(1)
	{
		int p = fp.P.begin();
		if( p == -1 )
			break;

		if( getBit(board,p/25,p%25)==BIT_UNKNOWN )
		{
			res = probe( fp , ls , board , p/25 , p%25);
			if( res == SOLVED || res == CONFLICT )
				return res;
		}
	}

	getSize(board);
	//}

	setBestPixel( fp , board );

	return INCOMP;
}


void setBestPixel( FullyProbe& fp , Board& board )
{
	auto max = make_tuple(0,0,0);
	double maxPixel = 0;

	Dual_for(i,j)
		if( getBit(board,i,j) == BIT_UNKNOWN )
		{
			for( int k = 0 ; k < 50 ; ++k )
			{
				fp.gp[i][j][0].data[k] &= board.data[k];
				fp.gp[i][j][1].data[k] &= board.data[k];
			}


			double ch = choose( fp.method , 
					getSize(fp.gp[i][j][1])-board.size ,
					getSize(fp.gp[i][j][0])-board.size );
			if( ch > maxPixel ) 
			{
				max = make_tuple(i,j, fp.gp[i][j][0].size > fp.gp[i][j][1].size ? 0 : 1);
				maxPixel = ch;
			}
		}

	//printf("select %d %d %lf\n" , get<0>(max) , get<1>(max) , maxPixel );
	fp.max_g0 = fp.gp[get<0>(max)][get<1>(max)][get<2>(max)];
	fp.max_g1 = fp.gp[get<0>(max)][get<1>(max)][!get<2>(max)];
}

#define vlog(x) (log(x+1)+1)
double choose( int method , int mp1 , int mp0 )
{
	//if(mp1<=0)mp1=0;
	//if(mp0<=0)mp0=0;
	//printf("mp1=%d mp0=%d\n",mp1,mp0);
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

	int p0 = probeG( fp ,ls ,pX ,pY ,BIT_ZERO ,board );
	if( p0 == SOLVED )
	{
		return SOLVED;
	}
	int p1 = probeG( fp ,ls ,pX ,pY ,BIT_ONE ,board );
	if( p1 == SOLVED )
	{
		return SOLVED;
	}

	if( p0==CONFLICT && p1==CONFLICT )
	{
		return CONFLICT;
	}
	else if( p1==CONFLICT )
	{
		board = fp.gp[pX][pY][0];
	}
	else if( p0==CONFLICT )
	{
		board = fp.gp[pX][pY][1];
	}
	else
	{
		for ( int i = 0 ; i < 50 ; ++i )
			board.data[i] = fp.gp[pX][pY][0].data[i] | fp.gp[pX][pY][1].data[i];
	}

	/*
	   for( int x = 0 ; x < 25 ; ++x )
	   {
	   uint64_t tmp = board.data[x] ^ old.data[x];
	   if( !tmp )  continue;
	   int pos = 0;
	   while( 0 != (pos=__builtin_ffsll(tmp)) )
	   {
	   pos--;
	   tmp &= tmp-1;
	   int y = pos>>1;
	   if( x!=pX && y!=pY )
	   fp.P.insert( x*25 + y );
	   }
	   }
	 */

	return INCOMP;
}

int probeG( FullyProbe& fp ,LineSolve& ls ,int pX ,int pY ,uint64_t pVal , Board &origin )
{
	pVal -= BIT_ZERO;
	Board newG = fp.gp[pX][pY][pVal];
	int newGstate = propagate(ls , newG );
	if( newGstate == SOLVED || newGstate == CONFLICT )
		return newGstate;

#ifdef USE_FP2
	for( int _x = 0 ; _x < 25 ; ++_x )
	{
		uint64_t tmp = newG.data[_x] ^fp.gp[pX][pY][pVal].data[_x];
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
				setBit( fp.gp[_x][_y][_v] , pX , pY , ( !(pVal==0) ? BIT_ZERO : BIT_ONE ) );
				/*
				   for( int i = 0 ; i < 50 ; ++i )
				   fp.gp[_x][_y][_v].data[i] &= fp.gp[pX][pY][!pVal].data[i];	
				 */
			}
		}
	}
#endif

	fp.gp[pX][pY][pVal] = newG;

	return newGstate;
}

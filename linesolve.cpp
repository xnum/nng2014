#include "linesolve.h"
#include <cstdio>

LineSolve::LineSolve() 
{
	init();
}

LineSolve::LineSolve(int* d,int pbn) 
{
	init();
	load(d,pbn);
}

void LineSolve::init()
{
	value1[0] = 0x0LL;
	value0[0] = (uint64_t)BIT_ZERO; 
	__SET( value1[0] , 0 , BIT_ZERO );

	for ( int i = 1 ; i < 28 ; ++i )
	{
		value0[i] = value0[i-1]<<2;
		value1[i] = value1[i-1];
		__SET( value1[i] , i , BIT_ONE );
	}

}

void LineSolve::load(int* d,int pbn)
{
	probN=pbn;
	memcpy( data , d , sizeof(int)*50*14 );

	MEMSET_ZERO(preFixTable);
	MEMSET_ZERO(low_bound);
	queryTable.clear();

	for ( int i = 0 ; i < 50 ; ++i )
	{
		int sum = 0;
		low_bound[i][0] = 0;

		for ( int j = 1 ; j <= data[i*14] ; ++j )
		{
			sum += data[i*14+j] + 1;
			low_bound[i][j] = sum-1;
		}

		needCalc[i] = 26 - sum + 1;

		preFixTable[i][0][0] = LS_YES;
		for ( int y = 0 ; y < 14 ; ++y )
			for ( int x = 0 ; x < low_bound[i][y] ; ++x )
				preFixTable[i][x][y] = LS_NO; 
	}
}



int propagate ( LineSolve& ls , Board& board )
{
	//if( checkBoard(board) ) return CONFLICT;

	uint64_t chkline = FILL;
	ls.lineNum = 0;

	while( 1 )
	{
		ls.lineNum = __builtin_ffsll(chkline);
		if( ls.lineNum-- == 0 ) break;
		chkline &= chkline-1;

		ls.line = getLine ( board , ls.lineNum ) << 4;

#ifdef CUT_BY_SIZE
		if( board.lastSize[ls.lineNum] == __builtin_popcountll(ls.line) )
			continue; 
#endif
		__SET( ls.line , 1 , BIT_ZERO );

#ifdef CUT_BY_CACHE
		uint64_t res;
		res = ls.queryTable.query( ls.lineNum , ls.line );

		if( res == Rbtree::NOT_FOUND ) {
#endif

			memmove( ls.fixTable , ls.preFixTable[ls.lineNum] , sizeof(ls.fixTable) );

			ls.newLine = 0LL;

			ls.lineNum *= 14;
			if ( LS_NO == fix ( ls , 26 , ls.data[ls.lineNum] ) )
			{
				ls.lineNum /= 14;
				ls.queryTable.insert( ls.lineNum , ls.line , Rbtree::ANS_ERR );
				return CONFLICT;
			}
			ls.lineNum /= 14;

#ifdef CUT_BY_CACHE
			ls.queryTable.insert( ls.lineNum , ls.line , ls.newLine );
		}
		else
		{
			ls.newLine = res;
			if( ls.newLine == Rbtree::ANS_ERR )
				return CONFLICT;
		}
#endif

		ls.newLine >>= 4;
		ls.line >>= 4;

#ifdef CUT_BY_SIZE
		board.lastSize[ls.lineNum] = __builtin_popcountll( ls.newLine );
#endif

		if( ls.line != ls.newLine )
		{
			board.data[ls.lineNum] = ls.newLine;

			uint64_t p = ls.line ^ ls.newLine;
			int x = 1;

			while ( x = __builtin_ffsll(p) , x-- != 0 )
			{
				uint64_t bit = (x&0x1)==0 ? BIT_ONE : BIT_ZERO;
				p &= p-1;
				x>>=1;

				if ( ls.lineNum < 25 )
				{
					chkline |= 0x1LL<<(x+25);
					__SET( board.data[x+25] , (ls.lineNum) , bit );
				}
				else
				{
					chkline |= 0x1LL<<x;
					__SET( board.data[x] , (ls.lineNum-25) , bit );
				}
			}
		}
	}

	//puts("leaving propagate");

	if( getSize(board) != 625 )
		return INCOMP;

	ls.solvedBoard = board;
	//printBoard( board,ls.probN );

	return SOLVED;
}

int fix ( LineSolve& ls , int i, int j )
{
	/*
	uint8_t &ret = ls.fixTable[i][j];

	if ( ret == LS_NANS )
	{
		ret = LS_NO;

		int dj = ls.data[ls.lineNum+j];
		int length = i - dj;
		uint64_t val0 = ls.value0[i];
		uint64_t val1 = ls.value1[dj] << (length << 1);

		if( ls.line&val0 )
			if( LS_YES==fix(ls,i-1,j) )
			{
				ls.newLine |= val0; 
				ret = LS_YES;
			}

		if( j )
			if( ls.line==(ls.line|val1) )
				if( LS_YES==fix(ls,length-1,j-1) )
				{
					ls.newLine |= val1;
					ret = LS_YES;
				}
	}

	return ret;
	*/
	
	for( int jp = 0 ; jp <= j ; ++jp )
	{
		const int dj = ls.data[ls.lineNum+jp];
		int ip = ls.low_bound[ls.lineNum/14][jp];

		uint64_t val0 = ls.value0[ip];
		int length = ip - dj;
		uint64_t val1 = ls.value1[dj] << (length << 1);

		const int ipp = ip+ls.needCalc[ls.lineNum/14];

		for( ; ip <= ipp ; ++ip , val0<<=2 , val1<<=2 , ++length )
		{
			if( ls.fixTable[ip][jp] != LS_NANS )
				continue;

			uint8_t ret = LS_NO;


				if( ls.line&val0 )
					if( LS_YES==ls.fixTable[ip-1][jp] )
					{
						ls.newLine |= val0; 
						ret = LS_YES;
					}

			if( 0<=jp-1 && 0 <= length-1 ) 
			{
				if( ls.line==(ls.line|val1) )
					if( LS_YES==ls.fixTable[length-1][jp-1] ) 
					{
						ls.newLine |= val1;
						ret = LS_YES;
					}
			}

			ls.fixTable[ip][jp] = ret;
		}
	}

	return ls.fixTable[i][j];
}


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

	//MEMSET_ZERO(preFixTable);
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

		/*
		preFixTable[i][0][0] = LS_YES;
		for ( int y = 0 ; y < 14 ; ++y )
			for ( int x = 0 ; x < low_bound[i][y] ; ++x )
				preFixTable[i][x][y] = LS_NO; 
				*/

	}
}



int propagate ( LineSolve& ls , Board& board )
{
	uint64_t chkline = 0;
	for( int i = 0 ; i < 50 ; ++i )
	{
		if( board.oldData[i] != board.data[i] )
			chkline |= 1LL << i;
	}
	uint64_t nextchk = 0LL;
	ls.lineNum = 0;

	while( 1 )
	{
		if( chkline == 0 )
		{
			if( nextchk == 0 )
				break;
			chkline = nextchk;
			nextchk = 0LL;
		}

		ls.lineNum = __builtin_ffsll(chkline)-1;
		chkline &= chkline-1;
		
		ls.line = getLine ( board , ls.lineNum ) << 4;

		__SET( ls.line , 1 , BIT_ZERO );

		uint64_t res;
		res = ls.queryTable.query( ls.lineNum , ls.line );

		if( res == Rbtree::NOT_FOUND ) 
		{
			ls.lineNum *= 14;
			//memmove( ls.fixTable , ls.preFixTable[ls.lineNum/14] , sizeof(ls.fixTable) );
			ls.newLine = 0LL;

			const int fixAns = fixBU ( ls , ls.data[ls.lineNum] );
			if ( LS_NO == fixAns )
			{
				ls.lineNum /= 14;
				ls.queryTable.insert( ls.lineNum , ls.line , Rbtree::ANS_ERR );
				return CONFLICT;
			}
			ls.lineNum /= 14;

			ls.queryTable.insert( ls.lineNum , ls.line , ls.newLine );
		}
		else
		{
			ls.newLine = res;
			if( ls.newLine == Rbtree::ANS_ERR )
				return CONFLICT;
		}

		ls.newLine >>= 4;
		ls.line >>= 4;

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
					nextchk |= 0x1LL<<(x+25);
					__SET( board.data[x+25] , (ls.lineNum) , bit );
				}
				else
				{
					nextchk |= 0x1LL<<x;
					__SET( board.data[x] , (ls.lineNum-25) , bit );
				}
			}
		}
	}

	memcpy(board.oldData,board.data,sizeof(board.oldData));

	if( getSize(board) != 625 )
		return INCOMP;

	ls.solvedBoard = board;

	return SOLVED;
}

/*
int fix ( LineSolve& ls , int i, int j )
{
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
}
*/

int fixBU ( LineSolve& ls , int j )
{
	const int i = 26;
	const int maxShift = ls.needCalc[ls.lineNum/14];
	uint64_t dpTable[27][14] = {};

	const uint64_t line = ls.line;
	int data[14] = {};
	int low_bound[14] = {};

	memcpy(data,&ls.data[ls.lineNum],sizeof(data));
	memcpy(low_bound,&ls.low_bound[ls.lineNum/14],sizeof(low_bound));

	dpTable[0][0] = BIT_ONE;

	for( int jp = 0 ; jp <= j ; ++jp )
	{
		const int dj = data[jp];
		int ip = low_bound[jp]+1;

		uint64_t val0 = (uint64_t)BIT_ZERO << (ip<<1); 
		int length = ip - dj;
		uint64_t val1 = ls.value1[dj] << (length << 1);
		length--;

		const int ipp = ip+maxShift;

		for( ; ip < ipp ; ++ip , val0<<=2 , val1<<=2 , ++length )
		{
			uint64_t currLine = 0;

			if( line&val0 )
				if( 0 != dpTable[ip-1][jp] )
					currLine |= val0 | dpTable[ip-1][jp]; 

			if( jp!=0 && line==(line|val1) )
				if( 0 != dpTable[length][jp-1] )
					currLine |= val1 | dpTable[length][jp-1];

			dpTable[ip][jp] = currLine;
		}
	}

	ls.newLine = dpTable[i][j];
	return dpTable[i][j] == 0 ? LS_NO : LS_YES;
}


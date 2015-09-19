#include "linesolve.h"
#include <cstdio>

LineSolve::LineSolve() 
{
	init();
}

LineSolve::LineSolve(int* d) 
{
	init();
	load(d);
}

void LineSolve::init()
{
	initialHash();

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

void LineSolve::load(int* d)
{
	MEMSET_ZERO(low_bound);

	for ( int i = 0 ; i < 50 ; ++i )
	{
		int sum = 0;
		low_bound[i][0] = 0;

		clue[i].count = d[i*14];
		for ( int j = 1 ; j <= d[i*14] ; ++j )
		{
			clue[i].num[j-1] = d[i*14+j];	
			sum += d[i*14+j] + 1;
			low_bound[i][j] = sum-1;
		}

		needCalc[i] = 26 - sum + 1;

		genHash(clue[i]);
	}
}



int propagate ( LineSolve& ls , Board& board )
{
	uint64_t chkline = 0;
	for( int i = 0 ; i < 50 ; ++i )
	{
		if( unlikely( board.oldData[i] != board.data[i] ) )
			chkline |= 1LL << i;
	}
	uint64_t nextchk = 0LL;
	ls.lineNum = 0;

	while( 1 )
	{
		if( unlikely( chkline == 0 ) )
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

		if( !findHash(ls.clue[ls.lineNum], ls.line, ls.newLine) )
		{
			ls.newLine = 0LL;

			const int fixAns = fixBU ( ls , ls.clue[ls.lineNum].count );
			if ( unlikely( LS_NO == fixAns ) )
			{
				return CONFLICT;
			}

			insertHash(ls.clue[ls.lineNum], ls.line, ls.newLine);
		}

		ls.newLine >>= 4;
		ls.line >>= 4;

		if( unlikely( ls.line != ls.newLine ) )
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

	if( unlikely( getSize(board) != 625 ) )
		return INCOMP;

	ls.solvedBoard = board;

	return SOLVED;
}

int fixBU ( LineSolve& ls , int j )
{
	const int i = 26;
	const int maxShift = ls.needCalc[ls.lineNum];
	uint64_t dpTable[14][27] = {};

	const uint64_t line = ls.line;
	int data[14] = {};
	int low_bound[14] = {};

	memcpy(data+1,&ls.clue[ls.lineNum].num,sizeof(int)*13);
	memcpy(low_bound,&ls.low_bound[ls.lineNum],sizeof(low_bound));

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
			if( likely( (line&val0) && dpTable[jp][ip-1] ) )
					dpTable[jp][ip] |= val0 | dpTable[jp][ip-1]; 

			if( unlikely( jp!=0 && line==(line|val1) && dpTable[jp-1][length] ))
					dpTable[jp][ip] |= val1 | dpTable[jp-1][length];
		}
	}

	ls.newLine = dpTable[j][i];
	return dpTable[j][i] == 0 ? LS_NO : LS_YES;
}


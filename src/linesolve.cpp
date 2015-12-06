#include "linesolve.h"
#include <cstdio>


const uint64_t value1[28] = {
	0x1ULL,
	0x9ULL,
	0x29ULL,
	0xA9ULL,
	0x2A9ULL,
	0xAA9ULL,
	0x2AA9ULL,
	0xAAA9ULL,
	0x2AAA9ULL,
	0xAAAA9ULL,
	0x2AAAA9ULL,
	0xAAAAA9ULL,
	0x2AAAAA9ULL,
	0xAAAAAA9ULL,
	0x2AAAAAA9ULL,
	0xAAAAAAA9ULL,
	0x2AAAAAAA9ULL,
	0xAAAAAAAA9ULL,
	0x2AAAAAAAA9ULL,
	0xAAAAAAAAA9ULL,
	0x2AAAAAAAAA9ULL,
	0xAAAAAAAAAA9ULL,
	0x2AAAAAAAAAA9ULL,
	0xAAAAAAAAAAA9ULL,
	0x2AAAAAAAAAAA9ULL,
	0xAAAAAAAAAAAA9ULL,
	0x2AAAAAAAAAAAA9ULL,
	0xAAAAAAAAAAAAA9ULL,
};

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
}

void LineSolve::load(int* d)
{
	MEMSET_ZERO(low_bound);

	for ( int i = 0 ; i < 50 ; ++i )
	{
		int sum = 0;

		clue[i].count = d[i*14];

		for ( int j = 1 ; j <= d[i*14] ; ++j )
		{
			clue[i].num[j-1] = d[i*14+j];	
			sum += d[i*14+j] + 1;
			low_bound[i][j] = sum-1;
		}

		genHash(clue[i]);
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
	int lineNum = 0;

	while( 1 )
	{
		if( chkline == 0 )
		{
			if( nextchk == 0 )
				break;
			chkline = nextchk;
			nextchk = 0LL;
		}

		lineNum = __builtin_ffsll(chkline)-1;
		chkline &= chkline-1;
		
		uint64_t line = getLine ( board , lineNum ) << 4;

		__SET( line , 1 , BIT_ZERO );
		uint64_t newLine = 0LL;

		if( !findHash(ls.clue[lineNum], line, newLine) )
		{
			newLine = 0LL;

			const int fixAns = fixBU ( ls , lineNum , line , ls.clue[lineNum].count , newLine );
			if ( unlikely( LS_NO == fixAns ) )
			{
				return CONFLICT;
			}

			insertHash(ls.clue[lineNum], line, newLine);
		}

		newLine >>= 4;
		line >>= 4;

		if( line != newLine )
		{
			board.data[lineNum] = newLine;

			uint64_t p = line ^ newLine;
			int x = 1;

			while ( x = __builtin_ffsll(p) , x-- != 0 )
			{
				uint64_t bit = (x&0x1)==0 ? BIT_ONE : BIT_ZERO;
				p &= p-1;
				x>>=1;

				if ( lineNum < 25 )
				{
					nextchk |= 0x1LL<<(x+25);
					__SET( board.data[x+25] , lineNum , bit );
				}
				else
				{
					nextchk |= 0x1LL<<x;
					__SET( board.data[x] , (lineNum-25) , bit );
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

int fixBU ( LineSolve& ls , int lineNum , const uint64_t& line , int j , uint64_t& newLine )
{
	const int i = 26;
	const int maxShift = 26 - ls.low_bound[lineNum][ls.clue[lineNum].count-1];
	uint64_t dpTable[14][27] = {};

	uint8_t data[14] = {};
	int low_bound[14] = {};

	memcpy(data+1,&ls.clue[lineNum].num,sizeof(data[0])*13);
	memcpy(low_bound,&ls.low_bound[lineNum],sizeof(low_bound));

	dpTable[0][0] = BIT_ONE;

	for( int jp = 0 ; jp <= j ; ++jp )
	{
		const int dj = data[jp];
		int ip = low_bound[jp]+1;

		uint64_t val0 = (uint64_t)BIT_ZERO << (ip<<1); 
		int length = ip - dj;
		uint64_t val1 = value1[dj] << (length << 1);
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

	newLine = dpTable[j][i];
	return dpTable[j][i] == 0 ? LS_NO : LS_YES;
}


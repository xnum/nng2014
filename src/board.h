#ifndef BOARD_H
#define BOARD_H

#include "cdef.h"
#include <stdint.h>

class Board
{
	public:
		uint64_t data[50];
		uint64_t oldData[50];
		uint16_t size;	

		Board();

		bool operator<(const Board& rhs) const
		{
			for( int i = 0 ; i < 25 ; ++i )
				if( data[i] < rhs.data[i] )
					return true;
			return false;
		}
		bool operator==(const Board& rhs) const
		{
			for( int i = 0 ; i < 25 ; ++i )
				if( data[i] != rhs.data[i] )
					return false;
			return true;
		}

};

inline void merge( Board& a,Board& b )
{
	for(int i = 0 ; i < 50 ; ++i)
		a.data[i] &= b.data[i];
}

inline int getSize( Board &board )
{
	board.size = 25*50;
	for ( int i = 0 ; i < 25 ; ++i )
		board.size -= __builtin_popcountll(board.data[i]);

	return board.size;
}

// only check if has illegal bit
inline int checkBoard( Board &b )
{
	for( int i = 0 ; i < 50 ; ++i )
	{
		uint64_t test = ( b.data[i] & 0x5555555555555555LL) | ( b.data[i]>>1 & 0x5555555555555555LL);
		if( test != 0x1555555555555LL )
			return 1;
	}
	return 0;
}

inline void setBit( Board& board , int x , int y , int val )
{
	__SET( board.data[x] , y , val );	
	__SET( board.data[y+25] , x , val );	
}

inline int getBit( const Board& board , int x , int y )
{
	return __GET( board.data[x] , y );
}

inline void setLine( Board& board , int line , uint64_t val )
{
	if( line < 25 )
	{
		board.data[line] = val;
		for ( int k = 0 ; k < 25 ; ++k )
		{
			__SE( board.data[k+25] , line ,  __GET( val , k ) );
			__builtin_prefetch( &board.data[k+26] , 1 );
		}
	}
	else
	{
		board.data[line] = val;
		for ( int k = 0 ; k < 25 ; ++k )
		{
			__SE( board.data[k] , (line-25) , __GET( val , k ) );
			__builtin_prefetch( &board.data[k+1] , 1 );
		}
	}
}

inline uint64_t getLine( Board& board , int x )
{
	return board.data[x];
}

void printBoard( char* , Board& , int );
void debugBoard( Board& );
int checkAns( Board& , int[] );


#endif

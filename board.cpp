/* lazy include */
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include "board.h"
using namespace std;

Board::Board()
{
	for( int i = 0 ; i < 50 ; ++i )
	{
		data[i] = ( ( 0x1LL << 50 ) - 0x1LL );
		oldData[i] = 0;
	}

	size = 0;
}

int checkAns( Board& board , int data[] )
{
	for( int i = 0 ; i < 25 ; ++i )
	{
		int n=0, c=0;
		for( int j = 0 ; j < 25 ; ++j )
		{
			uint64_t val = getBit(board,i,j);
			if(!(val==BIT_ONE
						|| val==BIT_ZERO ))
			{ 	
				printf("Pixel illegal(%" PRIu64 ").\n",val); 
				return 0; 
			}

			if( val==BIT_ONE )
				n++;
			else if( val==BIT_ZERO || j == 24 )
			{
				if( n != 0 )
				{
					if( data[(i)*14+c+1] != n )
					{
						printf("Error answer\n");
						return 0;
					}
					else
					{
						n = 0;
						c++;
					}
				}
			}
			else
			{
				printf("WTF");
				return 0;
			}
		}
	}

	for( int j = 0 ; j < 25 ; ++j )
	{
		int n = 0 , c = 0;
		for( int i = 0 ; i < 25 ; ++i )
		{
			uint64_t val = getBit(board,i,j);
			if( val==BIT_ONE )
				n++;
			else if( val==BIT_ZERO || i == 24 )
			{
				if( n != 0 )
				{
					if( data[(j+25)*14+c+1] != n )
					{
						printf("Error ans\n");
						return 0;
					}
					else
					{
						n=0;
						c++;
					}
				}
			}
			else
			{
				printf("WTF");
				return 0;
			}
		}
	}

	return 1;

}

void printBoard( char* fileName ,Board& board , int probN )
{
	FILE* out = fopen( fileName , "a+" );
	fprintf( out , "$%d\n",probN);
#ifdef DEBUG
	puts("ROW=============================");
#endif
	for( int i = 0 ; i < 25 ; ++i )
	{
		for( int j = 0 ; j < 25 ; ++j )
		{
			uint64_t val = __GET( board.data[j] , i );
			if( BIT_ZERO == val )
				fprintf( out , "0" );
			else if( BIT_ONE == val )
				fprintf( out , "1" );
			else 
				fprintf( out , "?" );

			if( j==24 )
				fprintf( out , "\n" );
			else
				fprintf( out , "\t" );
		}
	}
	fclose(out);
#ifdef DEBUG
	puts("================================");

	puts("MAP=============================");
	for( int i = 0 ; i < 25 ; ++i )
	{
		for( int j = 0 ; j < 25 ; ++j )
		{
			uint64_t val = __GET( board.data[i+25] , j );
			if( BIT_ZERO == val )
				putchar('O');
			if( BIT_ONE == val )
				putchar('@');
			if( BIT_UNKNOWN == val )
				putchar('_');
		}
		putchar('\n');
	}
	puts("================================");
	printf("\n\n");
#endif


}

void debugBoard( Board& board )
{
	puts("=--=============================");
	for( int i = 0 ; i < 25 ; ++i )
	{
		for( int j = 0 ; j < 25 ; ++j )
		{
			uint64_t val = __GET( board.data[i] , j );
			if( BIT_ZERO == val )
				printf( "O" );
			else if( BIT_ONE == val )
				printf( "@" );
			else if( BIT_UNKNOWN == val )
			{
				putchar('_');
			}
			else
			{
				putchar('?');
			}
			if( j==24 )
				printf( "\n" );
		}
	}

	puts("================================");
}


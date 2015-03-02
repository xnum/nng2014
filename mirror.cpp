#include "mirror.h"

#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;


int mirror :: checkType( Board a , Board b , int x , int y )
{
	uint64_t bit_a = getBit( a , x , y ) ,
			 bit_b = getBit( b , x , y ) ;
	if( bit_a == bit_b )
		return SAME;
	else if( bit_a == BIT_ZERO && bit_b == BIT_ONE )
		return REFLECT;
	else if( bit_a == BIT_ONE && bit_b == BIT_ZERO )
		return REFLECT;
	else if( bit_a == BIT_UNKNOWN && bit_b == BIT_UNKNOWN )
		return BOTH_U;
	else
		return MUTEX;
}

mirror :: mirror( FullyProbe& fp , Board &g , size_t threshold )
{
	vector<node> unk;

	Dual_for(i,j)
			if( getBit( g , i , j ) == BIT_UNKNOWN )
			{
				node tmp;
				tmp.x = i;
				tmp.y = j;
				unk.push_back( tmp );
			}

	size_t p = 0;
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
		Dual_for(i,j)
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

		if( set.size() <= threshold ) 
			continue;

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

	if( vec.size() < 3 )
		return;

	// sort but seems doesn't work
	for( auto v : vec )
	{
		getSize(v);
	}


	for( size_t i = 0 ; i < vec.size()-2 ; i+=2 )
	{
		int isize = vec[i].size + vec[i+1].size;
		for( size_t j = i+2 ; j < vec.size() ; j+=2 )
		{
			int jsize = vec[j].size + vec[j+1].size;
			if( jsize > isize )
			{
				Board tmp0 = vec[i];
				vec[i] = vec[j];
				vec[j] = tmp0;

				Board tmp1 = vec[i+1];
				vec[i+1] = vec[j+1];
				vec[j+1] = tmp1;
			}
		}
	}

}


bool mirror :: generatePattern( Board& b , int max )
{
	//FILE *t = fopen( "test.txt" , "a+" );

	if( vec.size() < 3 )
	{
		//fprintf( t , "0\n" );
		//fclose(t);
		return false;
	}
	else
	{
		//int a=0,bb=0;
		//fprintf( t , "1\t" );
		int perm = vec.size()/2;
		perm = perm >= max ? max : perm;
		
		for( int k = 0 ; k < (1<<perm) ; ++k )
		{
			Board cBoard = b;
			for( int p = 0 ; p < perm ; ++p )
				for( int o = 0 ; o < 50 ; ++o )
					cBoard.data[o] &= vec[p*2+((k&(1<<p))>>p)].data[o];
					
			if( 1==checkBoard(cBoard) )
			{
				//a++;
				continue;
			}
			//bb++;

			pattern.push_back(cBoard);
		}
		//fprintf( t , "%d\t%d\n" , a , bb );
		//fclose(t);
	}
	if( pattern.size() == 0 )
		return false;
	return true;
}

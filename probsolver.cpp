#include "probsolver.h"

int NonogramSolver::doSolve(int *data)
{
	Board b;
	ls.load(data,0);

	if( SOLVED != fp2( fp , ls , b ) )
	{
		finish = false;
		times = 0;

		dfs( fp , ls , b );  
	}

	return 0;
}

void NonogramSolver::setMethod(int n)
{
	fp.method = n;
}

void NonogramSolver::dfs( FullyProbe& fp , LineSolve& ls , Board b )
{
	times++;

#ifdef MIRROR
	if( times == 1 )
	{
		mirror t( fp , b , PATTERN_DROP_THRESHOLD );
		if( true == t.generatePattern(b , MAX_PERM) )
		{
			for( auto subBoard : t.pattern )
			{
				dfs( fp , ls , subBoard );
				if( finish == true )
					break;
			}
			return 0;
		}
	}
#endif

	int res = fp2( fp , ls , b );
	if( res == SOLVED )
	{
		finish = true;
		return;
	}

	if( res == CONFLICT )
		return;

	Board b1 = fp.max_g1;
	Board b0 = fp.max_g0;

	dfs( fp , ls , b0 );

	if( finish == true )
		return;

	dfs( fp , ls , b1 );
}



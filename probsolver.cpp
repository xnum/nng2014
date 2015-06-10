#include "probsolver.h"

int NonogramSolver::doSolve(int *data)
{
	Board b;
	ls.load(data,0);

	if( SOLVED != fp2( fp , ls , b ) )
	{
		finish = false;
		times = 0;
		thres = 100;

		dfs( fp , ls , b );  
	}

	if( finish != true )
	{
		printf("ALL CONFLICT\n");
		return 1;
	}

	return 0;
}

void NonogramSolver::setMethod(int n)
{
	fp.method = n;
}

void NonogramSolver::dfs( FullyProbe& fp , LineSolve& ls , Board b )
{
	queue.push_back(b);

	while(1)
	{
		times++;
		Board current = queue.back();
		queue.pop_back();

		/*
		if( times % thres == 0 )
		{
			if(times>=4900)printf("SW! %d\n",times);
			thres *= 3;
			queue.push_back(current);
			current = queue.front();
			queue.pop_front();
		}
		*/

#ifdef MIRROR
		if( times % 1000 == 1 )
		{
			//puts("Fix");
			mirror t( fp , current , PATTERN_DROP_THRESHOLD );
			if( true == t.generatePattern(current , MAX_PERM) )
			{
				for( auto subBoard : t.pattern )
					queue.push_back(subBoard);
				current = queue.back();
				queue.pop_back();
			}
		}
#endif

		int res = fp2( fp , ls , current );
		if( res == SOLVED )
		{
			finish = true;
			return;
		}

		if( res == CONFLICT )
			continue;

		queue.push_back(fp.max_g1);
		queue.push_back(fp.max_g0);

		if( finish == true )
			return;
	}
}



#include "probsolver.h"

int NonogramSolver::doSolve(int *data)
{
	Board b;
	ls.load(data);

	if( SOLVED != fp2( fp , ls , b ) )
	{
		finish = false;
		times = 0;
		thres = 20;
		sw = 0;

		dfs( fp , ls , b );  
	}

	if( finish != true )
	{
		printf("Error: ALL CONFLICT\n");
		return 0;
	}

	return 1;
}

void NonogramSolver::setMethod(int n)
{
	fp.method = n;
}

void NonogramSolver::dfs( FullyProbe& fp , LineSolve& ls , Board b )
{
	queue[sw].push_back(b);

	while(1)
	{
		times++;
		if( queue[sw].size() == 0 )
			sw = sw==1?0:1;
		Board current = queue[sw].back();
		queue[sw].pop_back();

		if( 0 ) // times % thres == 0 )
		{
			//if(times>=1000)printf("SW! %d\n",times);
			thres *= 2;
			int nsw = sw==1 ? 0 : 1;
			if( queue[nsw].size() != 0 )
			{
				queue[sw].push_back(current);
				current = queue[sw].front();
				queue[sw].pop_front();
			}
			sw = nsw;
		}

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

		queue[sw].push_back(fp.max_g1);
		queue[sw].push_back(fp.max_g0);

		if( finish == true )
			return;
	}
}



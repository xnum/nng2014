#include "probsolver.h"

extern int size,mpi_rank;

int NonogramSolver::doSolve(int *data)
{
	Board b;
	ls.load(data);
	fp.clear();

	auto rc = fp2( fp , ls , b );
	if( CONFLICT == rc )
		printf("WTF!\n");
	if( SOLVED != rc )
	{
		search_finish = false;
		times = 0;
		thres = 20;
		sw = 0;
		max_depth = 0;
		MEMSET_ZERO(depth_rec);
		//dfs( fp , ls , b );  
		dfs_stack(fp,ls,b,0);

		if( search_finish != true )
		{
			printf("%d Error: ALL CONFLICT in %d,%d\n",mpi_rank,times,max_depth);
			return 0;
		}


	}

	return 1;
}

void NonogramSolver::setMethod(int n)
{
	fp.method = n;
}

void NonogramSolver::dfs_stack(FullyProbe& fp,LineSolve& ls,Board b,int depth)
{
	if( depth > 625 )
	{
		puts("Aborted: depth > 625");
		exit(1);
	}

	if( depth > max_depth )
		max_depth = depth;

	//depth_rec[depth]++;
	times++;
	int res = fp2( fp , ls , b );
	if( res == SOLVED )
	{
		//printf("== depth:%d(%d)\twidth:%d\n",depth,max_depth,depth_rec[depth]);
		search_finish = true;
		return;
	}

	if( res == CONFLICT )
	{
		return;
	}
    
	auto result = getBestPixel( fp , b , fp.method );
	Board b0 = fp.gp[get<0>(result)][get<1>(result)][get<2>(result)];
	Board b1 = fp.gp[get<0>(result)][get<1>(result)][!get<2>(result)];

	dfs_stack(fp,ls,b0,depth+1);
	if( search_finish == true )
		return;

	dfs_stack(fp,ls,b1,depth+1);
}

void NonogramSolver::dfs( FullyProbe& fp , LineSolve& ls , Board b )
{
	queue[sw].push_back(b);

	while(1)
	{
		//cout << queue[sw].size() << endl;
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
			search_finish = true;
			return;
		}

		if( res == CONFLICT )
			continue;

		queue[sw].push_back(fp.max_g1);
		queue[sw].push_back(fp.max_g0);

		if( search_finish == true )
			return;
	}
}



#include "probsolver.h"

int NonogramSolver::doSolve(int *data)
{
	Board b;
	ls.load(data);
	fp.clear();

	if( SOLVED != fp2( fp , ls , b ) )
	{
		finish = false;
		times = 0;
		thres = 20;
		sw = 0;
		max_depth = 0;
		MEMSET_ZERO(depth_rec);
		dfs( fp , ls , b );  
		//dfs_stack(fp,ls,b,0);
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
			finish = true;
			return;
		}

		if( res == CONFLICT )
			return;

		Board b1 = fp.max_g1;
		Board b0 = fp.max_g0;

		dfs_stack(fp,ls,b0,depth+1);
		if( finish == true )
			return;

		dfs_stack(fp,ls,b1,depth+1);
}

void NonogramSolver::dfs( FullyProbe& fp , LineSolve& ls , Board b )
{
  queue.clear();
	queue.push_back(b);

	while(1)
	{
		times++;
    if(queue.size() == 0) abort();
		Board current = queue.back();
		queue.pop_back();

		int res = fp2( fp , ls , current );
		if( res == SOLVED )
		{
			finish = true;
			return;
		}

		if( res == CONFLICT )
    {
#ifdef ACP
      deprecated_boards.push_back(current);
      if(deprecated_boards.size() >= thres) {
        thres *= 1.5;

        Board base;
        Dual_for(i,j) {
          Board d0 = deprecated_boards[0];
          int bit = getBit(d0, i, j);
          for(int k = 1; k < deprecated_boards.size(); ++k) {
            Board dn = deprecated_boards[k];
            if(bit == BIT_UNKNOWN) break;
            if(bit != getBit(dn, i, j)) bit = BIT_UNKNOWN;
          }

          if(bit != BIT_UNKNOWN) {
            setBit(base, i, j, bit);
          }
        }

        Board creator = queue.front();
        int size_o = getSize(creator);
        Dual_for(i,j) {
          if(getBit(creator, i, j) == BIT_UNKNOWN) {
            if(getBit(base, i, j) != BIT_UNKNOWN) {
              setBit(creator, i, j, getBit(base, i, j) == BIT_ONE ? BIT_ZERO : BIT_ONE);
            }
          }
        }

        int size_n = getSize(creator);
        if(size_o < size_n)
          queue.push_back(creator);

        deprecated_boards.clear();
      }
#endif
			continue;
    }

		queue.push_back(fp.max_g1);
		queue.push_back(fp.max_g0);

		if( finish == true )
			return;
	}
}



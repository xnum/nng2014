#include "probsolver.h"

extern int size,mpi_rank;
extern int probNN;
void deepSearch( FullyProbe& fp, LineSolve& ls, Board b, int depth );

static vector<tuple<int, int> > selectPixel;
static int minDepth = 625;
static bool first = true;

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
        Board bc = b;
		search_finish = false;
		times = 0;
		thres = 20;
		sw = 0;
		max_depth = 0;
		MEMSET_ZERO(depth_rec);
		//dfs( fp , ls , b );  
        selectPixel.clear();
		dfs_stack(fp,ls,b,0);

        ls.load(data);
        fp.clear();
        minDepth = 625;
        selectPixel.clear();

        deepSearch(fp,ls,bc,0);

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

void printPath()
{
    FILE* out;
    if( first == true ) {
        first = false;
        out = fopen( string("path"+to_string(mpi_rank)+".txt").c_str(), "w" ); 
    }
    else
        out = fopen( string("path"+to_string(mpi_rank)+".txt").c_str(), "a+" ); 

    fprintf(out,"prob %d depth %lu\n",probNN,selectPixel.size());
    for( const auto& pixel : selectPixel ) {
        int i,j;
        tie(i, j) = pixel;
        fprintf(out,"%d %d\t",i,j);
    }
	fprintf(out,"\n");
    fflush(out);
    fclose(out);
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
        printPath();
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

    selectPixel.emplace_back(make_tuple(get<0>(result), get<1>(result)));

	dfs_stack(fp,ls,b0,depth+1);
	if( search_finish == true )
		return;

	dfs_stack(fp,ls,b1,depth+1);

    selectPixel.pop_back();
}

void NonogramSolver::deepSearch( FullyProbe& fp, LineSolve& ls, Board b, int depth )
{
	if( depth > max_depth )
		return;

    int res = fp2( fp, ls, b );
    if( res == SOLVED )
    {
		if( minDepth > depth ) {
			minDepth = depth;
			printPath();
		}
        return;
    }

	if( res == CONFLICT )
	{
		return;
	}
     
    if( depth > minDepth )
        return;

	Dual_for(i,j)
    if( getBit( b,i,j ) == BIT_UNKNOWN )
    {
        Board b0 = fp.gp[i][j][0];
        Board b1 = fp.gp[i][j][1];

        selectPixel.emplace_back(make_tuple(i,j));

        deepSearch(fp,ls,b0,depth+1);
        deepSearch(fp,ls,b1,depth+1);

        selectPixel.pop_back();
    }
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



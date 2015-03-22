/*
 * backtrack version
 *
 *
 *
 *
 *
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "board.h"
#include "scanner.h"
#include "linesolve.h"
#include "fullyprobe.h"
#include "help.h"
#include "mirror.h"

using namespace std;

FullyProbe fp;
int* inputData;
int probData[50*14];
bool finish = false;
int times = 0;


void dfs( FullyProbe& fp , LineSolve& ls , Board b )
{
    if( times == DFS_MAX_TIMES )
    {
        finish = true;
        return;
    }

    times++;

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

int main(int argc , char *argv[])
{
    int method=CH_MUL;
    if( argc==2 )
        method = argv[1][0]-'0';
    fp.method = method;

    char logName[50] = {};
    sprintf( logName ,"log_sorted_P%d_T%d.txt" ,MAX_PERM ,PATTERN_DROP_THRESHOLD );
    printLog(logName, method);

    clearFile(OUTPUT_FILE_NAME);

    inputData = allocMem(1001*50*14);
    readFile(inputData);

    time_t start_time = time(NULL);
    clock_t start;

    LineSolve ls;

    for( int probN = PROBLEM_START ; probN <= PROBLEM_END ; ++probN )
    {
        start = clock();

        getData( inputData ,probN ,probData );
        ls.load(probData,probN);

        Board b;

        if( SOLVED != fp2( fp , ls , b ) )
        {
            finish = false;
            times = 0;

#ifdef MIRROR
			mirror t( fp , b , PATTERN_DROP_THRESHOLD );
			if( true == t.generatePattern(b , MAX_PERM) )
			{
				for( auto subBoard : t.pattern )
				{
					dfs( fp , ls , subBoard );
					if( finish == true )
						break;
				}
			}
#endif
            if( !finish )
				dfs( fp , ls , b );  
        }

        printf ( "$%3d\ttime:%3.4fs\ttotal:%4lds\t%6d\n" , probN
                , ( double ) ( clock() - start ) / CLOCKS_PER_SEC
                , time ( NULL ) - start_time , times
               );

        FILE* log = fopen( logName , "a+" );
        fprintf ( log , "$%3d\ttime:%3.4fs\ttotal:%4lds\t%6d\n" , probN
                , ( double ) ( clock() - start ) / CLOCKS_PER_SEC
                , time ( NULL ) - start_time , times
                );
        fclose(log);
    }

	//expandInputFile("inputFP2.txt");
	//expandInputFile("inputDFS.txt");

    delete[] inputData;

    return 0;
}



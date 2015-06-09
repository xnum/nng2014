#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "probsolver.h"

using namespace std;

int main(int argc , char *argv[])
{
	int *inputData;
	int probData[50*14];

	Options option;
	int rc = parseOptions(argc, argv, option);
	if( rc )
	{
		printf("\nAborted: Illegal Options.\n");
		return 0;
	}

	option.print();

    //fp.method = option.method;

    char logName[100] = {};
	if( option.keeplog && (rc = genLog( option, logName, 100 )) )
	{
		printf("open log(%s) and write info failed\n",logName);
		return 0;
	}

    clearFile(option.outputFileName);

    inputData = allocMem(1001*50*14);
    readFile(inputData);

    time_t start_time = time(NULL);
	clock_t start_clock = clock();
    clock_t start;

	NonogramSolver nngSolver;
	nngSolver.setMethod(option.method);

    for( int probN = option.problemStart ; probN <= option.problemEnd ; ++probN )
    {
        start = clock();

        getData( inputData ,probN ,probData );

		nngSolver.doSolve(probData);

		Board ans = nngSolver.getSolvedBoard();

		if( option.selfCheck && !checkAns(ans, probData) )
		{
			printf("Fatal Error: Answer not correct\n");
			return 1;
		}

		printBoard(ans, probN);
		
		if(!option.simple)
		{
			printf ( "$%3d\ttime:%4lfs\n" , probN
					, ( double ) ( clock() - start ) / CLOCKS_PER_SEC );
		}
		else
		{
			if( probN%100==0 )
				printf("%3d\t%4ld\t%11.6lf\n",probN-0,time(NULL)-start_time,(double)(clock()-start_clock)/CLOCKS_PER_SEC);
		}

		if(option.keeplog)
		{
			FILE* log = fopen( logName , "a+" );
			fprintf ( log , "%3d\t\t%11.6lf\n" , probN
					, ( double ) ( clock() - start ) / CLOCKS_PER_SEC );
			fclose(log);
		}
    }

	printf("Total:\n%4ld\t%11.6lf\n",time(NULL)-start_time,(double)(clock()-start_clock)/CLOCKS_PER_SEC);
	if(option.keeplog)
	{
		FILE* log = fopen( logName , "a+" );
		fprintf(log,"Total:\n%4ld\t%11.6lf\n",time(NULL)-start_time,(double)(clock()-start_clock)/CLOCKS_PER_SEC);
		fclose(log);
	}

    delete[] inputData;

    return 0;
}



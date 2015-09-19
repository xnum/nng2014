#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "probsolver.h"
#include "options.h"

using namespace std;

int main(int argc , char *argv[])
{
	Options option;
	if( !option.readOptions(argc, argv) )
	{
		printf("\nAborted: Illegal Options.\n");
		return 0;
	}

	option.print();

	char logName[101] = {};
	if( option.genLogFile(logName, 100) )
	{
		printf("\nopen log(%s) and write info failed\n",logName);
		return 0;
	}

	clearFile(option.outputFileName);

	int *inputData;
	int probData[50*14];
	inputData = allocMem(1001*50*14);
	readFile(inputData);

	time_t start_time = time(NULL);
	clock_t start_clock = clock();
	clock_t start;

	NonogramSolver nngSolver;
	nngSolver.setMethod(option.method);

	vector<Board> answer;
	answer.resize(option.problemEnd-option.problemStart+1);
	for( int probN = option.problemStart ; probN <= option.problemEnd ; ++probN )
	{
		start = clock();

		getData( inputData ,probN ,probData );

		if( nngSolver.doSolve(probData) )
			break;

		Board ans = nngSolver.getSolvedBoard();

		if( option.selfCheck && !checkAns(ans, probData) )
		{
			printf("Fatal Error: Answer not correct\n");
			return 1;
		}

		answer[probN-option.problemStart] = ans;

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

	printf("Write answer to %s\n",option.outputFileName);
	for( int probN = option.problemStart, i = 0 ; probN <= option.problemEnd ; ++probN, ++i )
	{
		printBoard(option.outputFileName, answer[i], probN);
	}
	printf("Write done!\n");

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



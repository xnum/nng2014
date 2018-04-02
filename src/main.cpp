#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "probsolver.h"
#include "options.h"

using namespace std;

/*
 * startClock is from program start solving
 * thisClock is from current problem start solving
 *
 * clock_t is high resolution but overflow after 3600s
 * time_t support bigger range but only count by seconds
 */
void writePerDuration(const Options& option, int probN, time_t startTime, clock_t thisClock , clock_t startClock, int times )
{
		if(!option.simple)
		{
			printf ( "$%3d\ttime:%4lfs\ttotal:%ld\t%d\n" , probN , (double)(clock()-thisClock)/CLOCKS_PER_SEC, time(NULL)-startTime, times);
		}
		else
		{
			if( probN%100==0 )
				printf("%3d\t%4ld\t%11.6lf\n",probN,time(NULL)-startTime,(double)(clock()-startClock)/CLOCKS_PER_SEC);
		}

		if(option.keeplog)
		{
			FILE* log = fopen( option.logFileName , "a+" );
			fprintf ( log , "%3d\t\t%11.6lf\n" , probN
					, (double)(clock()-thisClock)/CLOCKS_PER_SEC);
			fclose(log);
		}
}

void writeTotalDuration(const Options& option, time_t startTime, clock_t startClk )
{
	printf("Total:\n%4ld\t%11.6lf\n",time(NULL)-startTime,(double)(clock()-startClk)/CLOCKS_PER_SEC);
	if(option.keeplog)
	{
		FILE* log = fopen( option.logFileName , "a+" );
		fprintf(log,"Total:\n%4ld\t%11.6lf\n",time(NULL)-startTime,(double)(clock()-startClk)/CLOCKS_PER_SEC);
		fclose(log);
	}
}

int main(int argc , char *argv[])
{
	Options option;
	if(!option.readOptions(argc, argv))
	{
		printf("\nAborted: Illegal Options.\n");
		return 0;
	}

	if(option.genLogFile())
	{
		printf("\nopen log(%s) and write info failed\n",option.logFileName);
		return 0;
	}

	clearFile(option.outputFileName);

	int *inputData;
	int probData[50*14];
	inputData = allocMem(1001*50*14);
	readFile(option.inputFileName,inputData);

	time_t startTime = time(NULL);
	clock_t startClk = clock();
	clock_t thisClk;

	NonogramSolver nngSolver;
	nngSolver.setMethod(option.method);
	initialHash();

	vector<Board> answer;
	answer.resize(option.problemEnd-option.problemStart+1);
	for( int probN = option.problemStart ; probN <= option.problemEnd ; ++probN )
	{
		thisClk = clock();

		getData( inputData ,probN ,probData );

		if( !nngSolver.doSolve(probData) )
			return 1;

		Board ans = nngSolver.getSolvedBoard();

		if( option.selfCheck && !checkAns(ans, probData) )
		{
			printf("Fatal Error: Answer not correct\n");
			return 1;
		}

		answer[probN-option.problemStart] = ans;

		writePerDuration(option,probN,startTime,thisClk,startClk, nngSolver.times);
	}
	delete[] inputData;

	for( int probN = option.problemStart, i = 0 ; probN <= option.problemEnd ; ++probN, ++i )
		printBoard(option.outputFileName, answer[i], probN);

	writeTotalDuration(option,startTime,startClk);

	return 0;
}



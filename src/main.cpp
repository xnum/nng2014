#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "mpi.h"

#include "probsolver.h"
#include "options.h"

using namespace std;

int size = 0, mpi_rank = 0;

/*
 * startClock is from program start solving
 * thisClock is from current problem start solving
 *
 * clock_t is high resolution but overflow after 3600s
 * time_t support bigger range but only count by seconds
 */
void writePerDuration(const Options& option, int probN, time_t startTime, clock_t thisClock , clock_t startClock )
{
		if(!option.simple)
		{
			printf ( "$%3d\ttime:%4lfs\ttotal:%ld\n" , probN , (double)(clock()-thisClock)/CLOCKS_PER_SEC, time(NULL)-startTime);
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
	MPI_Init(&argc ,&argv);
	double mpi_time = MPI_Wtime();

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

	/* MPI part */
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

	int numProb = (option.problemEnd - option.problemStart + 1) / size;
	const int start = option.problemStart + numProb * mpi_rank;
	const int end = numProb * (mpi_rank + 1);
	fprintf( stderr , "ID:%d Run %4d~%4d = %4d\n",mpi_rank,start,end,numProb );

	Board answer[251];
	for( int probN = start ; probN <= end ; ++probN )
	{
		thisClk = clock();

		getData( inputData ,probN ,probData );

		if( !nngSolver.doSolve(probData) )
			return 1;

		Board ans = nngSolver.getSolvedBoard();

		if( option.selfCheck && !checkAns(ans, probData) )
		{
			printf("ID:%d Fatal Error: Answer not correct\n",mpi_rank);
			return 1;
		}

		if( 0 > probN-start || probN-start >= 251 )
			fprintf( stderr , "Warning: Out of bound %d\n", probN-start );
		answer[probN-start] = ans;

		//writePerDuration(option,probN,startTime,thisClk,startClk);
	}

	printf("ID:%d Run completed, Wait barrier, Time:%lf\n",mpi_rank,MPI_Wtime()-mpi_time);

	MPI_Barrier(MPI_COMM_WORLD);

	if( mpi_rank == 0 ) // as master
	{
		for( int i = option.problemStart ; i <= option.problemEnd ; ++i )
		{
			MPI_Status status;
			Board b;
			if( start <= i && i <= end )
			{
				b = answer[i-start];
			}
			else
			{
				MPI_Recv(b.data ,50 ,MPI_UNSIGNED_LONG_LONG ,MPI_ANY_SOURCE ,i ,MPI_COMM_WORLD ,&status);
			}

			getData( inputData ,i ,probData );
			if( !checkAns(b,probData) )
			{
				fprintf(stderr,"Fatel Error: Answer received is wrong\n");
			}

			printBoard(option.outputFileName, b, i);
		}
	}
	else // as slave
	{
		for( int i = 0 ; i < numProb ; ++i ) 
			MPI_Send(answer[i].data ,50 ,MPI_UNSIGNED_LONG_LONG ,0 ,i+start ,MPI_COMM_WORLD);
	}

	printf("ID:%d All completed, Wait barrier, Time:%lf\n",mpi_rank,MPI_Wtime()-mpi_time);

	delete[] inputData;
	MPI_Finalize();
	return 0;
}



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
    /* Timer */
	double mpi_time = MPI_Wtime();

	time_t startTime = time(NULL);
	clock_t startClk = clock();
	clock_t thisClk;

    /* argument parsing */
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

    /* program init */
	int *inputData;
	int probData[50*14];
	inputData = allocMem(1001*50*14);
	readFile(option.inputFileName,inputData);

	NonogramSolver nngSolver;
	nngSolver.setMethod(option.method);
	initialHash();

	/* MPI part */
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

	Board answer[1001];
    /* single mode */
	if( mpi_rank == 0 && size == 1 )
	{
		for( int i = option.problemStart ; i <= option.problemEnd ; ++i )
        {
			getData( inputData ,i ,probData );

			if( !nngSolver.doSolve(probData) )
				return 1;

			Board ans = nngSolver.getSolvedBoard();

			if( option.selfCheck && !checkAns(ans, probData) )
			{
				printf("Fatal Error: Answer not correct\n");
				return 1;
			}

            answer[i] = ans;
        }
	}
    else // multi mode
    {
        int doneNum = 0;
        if( mpi_rank == 0 ) // master
        {
            runAsMaster(option,size,answer);
        }
        else // slave
        {
            while(1)
            {
                int probN = RecvFromMaster();
                if( probN == -1 )
                    break;

                getData( inputData ,probN ,probData );

                if( !nngSolver.doSolve(probData) )
                    return 1;

                Board ans = nngSolver.getSolvedBoard();

                if( option.selfCheck && !checkAns(ans, probData) )
                {
                    printf("ID:%d Fatal Error: Answer not correct\n",mpi_rank);
                    return 1;
                }

                sendToMaster(ans, probN);
            }
        }
    }

	printf("ID:%d Run completed, Wait barrier, Time:%lf\n",mpi_rank,MPI_Wtime()-mpi_time);
	MPI_Barrier(MPI_COMM_WORLD);

	if( mpi_rank == 0 )
	{
		for( int i = option.problemStart ; i <= option.problemEnd ; ++i )
			printBoard(option.outputFileName, answer[i], i);

		printf("ID:%d All completed, Wait barrier, Time:%lf\n",mpi_rank,MPI_Wtime()-mpi_time);
	}

	delete[] inputData;
	MPI_Finalize();
	return 0;
}



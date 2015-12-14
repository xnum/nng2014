#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "mpi.h"
#include "Worker.h"
#include "probsolver.h"
#include "options.h"

using namespace std;

int size = 0, mpi_rank = 0;
int live_proc = 0;
volatile int iamdone = 1;

int MPI_MyRecv(void *buff, int count, MPI_Datatype datatype, 
		int from, int tag, MPI_Comm comm, MPI_Status *status) {

	int flag, nsec_start=1000, nsec_max=100000;
	struct timespec ts;
	MPI_Request req;

	ts.tv_sec = 0;
	ts.tv_nsec = nsec_start;

	PMPI_Irecv(buff, count, datatype, from, tag, comm, &req);
	do {
		nanosleep(&ts, NULL);
		ts.tv_nsec *= 2;
		ts.tv_nsec = (ts.tv_nsec > nsec_max) ? nsec_max : ts.tv_nsec;
		PMPI_Request_get_status(req, &flag, status);
	} while (!flag);

	return (*status).MPI_ERROR;
}

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

	if( mpi_rank == 0 && size == 1 )
	{
		puts("SC mode");
		return 5;
	}

	live_proc = 1;

	Board answer[1001]; // only master use this
	int doneNum = 0;
	if( mpi_rank == 0 ) // master
	{
		int probN = option.problemStart;

		int sentSlave = 0; // to record how many slave got an terminate signal
		while( 1 )
		{
			if( probN > option.problemEnd ) // all problem is sent
			{
				probN = 5566;
				if( sentSlave == size-1 && doneNum == option.problemEnd-option.problemStart+1 )
					break;
				else
				{
					printf("all sent but doneNum = %d\n",doneNum);
				}
			}

			MPI_Status status;
			Board b;
			MPI_MyRecv(b.data ,50 ,MPI_UNSIGNED_LONG_LONG ,MPI_ANY_SOURCE ,MPI_ANY_TAG ,MPI_COMM_WORLD ,&status);
			if( status.MPI_TAG == 0 )  // ask for a new problem
			{
				if( probN == 5566 )
				{
					sentSlave++;
					live_proc++;
				}
				printf("%d goto %d\n",status.MPI_SOURCE,probN);
				// send back with probN as TAG
				MPI_Send(b.data ,50 ,MPI_UNSIGNED_LONG_LONG ,status.MPI_SOURCE ,probN ,MPI_COMM_WORLD);
				// point to next
				probN++;
			}
			else // retrieve a solved board
			{
				printf("recv %d from %d\n",status.MPI_TAG,status.MPI_SOURCE);
				if( 1 <= status.MPI_TAG && status.MPI_TAG <= 1000 ) // for a legal tag
				{ // check answer
					getData( inputData ,status.MPI_TAG ,probData );
					if( !checkAns(b,probData) )
					{
						fprintf(stderr,"Fatel Error: Answer received is wrong\n");
					}
					answer[status.MPI_TAG] = b;
					doneNum++;
				}
				else
				{
					fprintf(stderr,"Illegal TAG = %d\n",status.MPI_TAG);
				}
			}
		}

	}
	else // slave
	{
		while(1)
		{
			MPI_Status status;
			Board b;
			MPI_Send(b.data ,50 ,MPI_UNSIGNED_LONG_LONG ,0 ,0 ,MPI_COMM_WORLD);
			MPI_Recv(b.data ,50 ,MPI_UNSIGNED_LONG_LONG ,0 ,MPI_ANY_TAG ,MPI_COMM_WORLD ,&status);
			int probN = status.MPI_TAG;
			printf("ID:%d got %d\n",mpi_rank,probN);
			if(probN == 5566)
			{
				printf("ID:%d Got exit tag So exit\n",mpi_rank);
				break;
			}

			getData( inputData ,probN ,probData );

			if( !nngSolver.doSolve(probData) )
				return 1;

			Board ans = nngSolver.getSolvedBoard();

			if( option.selfCheck && !checkAns(ans, probData) )
			{
				printf("ID:%d Fatal Error: Answer not correct\n",mpi_rank);
				return 1;
			}
			MPI_Send(ans.data ,50 ,MPI_UNSIGNED_LONG_LONG ,0 ,probN ,MPI_COMM_WORLD);
		}
	}

	iamdone = 0;
	printf("ID:%d Run completed, Wait barrier, Time:%lf\n",mpi_rank,MPI_Wtime()-mpi_time);
	//MPI_Barrier(MPI_COMM_WORLD);

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



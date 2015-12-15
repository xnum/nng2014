#include "Worker.h"

FullyProbe* fp;
LineSolve* ls;
Board workBoard;
pthread_rwlock_t boardLock;
pthread_mutex_t bitLock[32];
int probeResult[32];
volatile uint8_t busyBit[32];
volatile uint8_t readyBit[32];
volatile uint8_t runBit = 1;
uint16_t tasks[32][625];
uint16_t taskNum[32];

extern int mpi_rank;

void* run(void *arg) {
	long id = (long)arg;

	//printf("Thread run as %n",id);
	while(runBit) {
		// wait for data ready
		while( readyBit[id] == 0 ) // == 0
		{
			if( runBit == 0 )
				break;
			// if no sleep, program deadlock
			// if has sleep, works but very slow
			//printf("%d] Thread Busy[%d] = %d , Ready[%d] = %d\n",mpi_rank,id,busyBit[id],id,readyBit[id]);	
			//usleep(1000);
		}

		//printf("Thread Wake up\n");

		// start running , set busy
		//pthread_mutex_lock(&bitLock[id]);
		readyBit[id] = 0; // 0
		busyBit[id] = 1; // 1
		//pthread_mutex_unlock(&bitLock[id]);

		//printf("%d] Thread Busy[%d] = %d , Ready[%d] = %d\n",mpi_rank,id,busyBit[id],id,readyBit[id]);	

		probeResult[id] = INCOMP;

		for( int i = 0 ; i < taskNum[id] ; ++i )
		{
			Board board;

			pthread_rwlock_rdlock(&boardLock);
			board = workBoard;
			pthread_rwlock_unlock(&boardLock);

			int p = tasks[id][i];	
			int res = probe( *fp , *ls , board , p/25 , p%25);
			if( res == SOLVED || res == CONFLICT ) {
				probeResult[id] = res;
				break;
			}

			pthread_rwlock_wrlock(&boardLock);
			for( int i = 0 ; i < 50 ; ++i ) 
				workBoard.data[i] &= board.data[i];
			pthread_rwlock_unlock(&boardLock);
		}

		//pthread_mutex_lock(&bitLock[id]);
		readyBit[id] = 0;
		busyBit[id] = 0;
		//printf("%d] busy[%d] = %d\n",mpi_rank,id,busyBit[id]);
		//pthread_mutex_unlock(&bitLock[id]);
	}

	pthread_exit(NULL);
}

Worker::Worker() {
	concurrent = 0;
	fp = NULL;
	ls = NULL;
	MEMSET_ZERO(probeResult);
	//MEMSET_ZERO(busyBit);
	//MEMSET_ZERO(readyBit);
	for( int i = 0 ; i < 32 ; ++i )
	{
		busyBit[i] = 0;
		readyBit[i] = 0;
		bitLock[i] = PTHREAD_MUTEX_INITIALIZER;
	}

	pthread_rwlock_init(&boardLock,NULL);
	extendThread();
}

Worker::~Worker() {
	pthread_rwlock_destroy(&boardLock);
}

void Worker::kill() {
	runBit = 0;
}

// external function call me to assign task
int Worker::setAndRun(FullyProbe* f,LineSolve* l,Board board) {
	extendThread();
	// 0. init
	fp = f;
	ls = l;
	workBoard = board;
	MEMSET_ZERO(taskNum);

	int count = 0;

	// 1. dispatch works
	while(1)
	{
		// scan points
		int p = (*fp).P.begin();
		if( p == -1 )
			break;

		// assign works
		if( getBit(board,p/25,p%25)==BIT_UNKNOWN )
		{
			int id = count%concurrent;
			tasks[id][taskNum[id]] = p;
			taskNum[id]++;
			count++;
		}
	}

	// 2. proceed
	for( int i = 0 ; i < concurrent ; ++i ) {
		//pthread_mutex_lock(&bitLock[i]);
		readyBit[i] = 1;
		busyBit[i] = 1;
		//pthread_mutex_unlock(&bitLock[i]);
		//printf("%d] Set Busy[%d] = %d , Ready[%d] = %d\n",mpi_rank,i,busyBit[i],i,readyBit[i]);	
	}

	// 3. wait
	while(1) {

		bool test = false;
		for( int i = 0 ; i < concurrent ; ++i ) {
			//printf("%d] Test Busy[%d] = %d , Ready[%d] = %d\n",mpi_rank,i,busyBit[i],i,readyBit[i]);	
			if( busyBit[i] == 1 )
				test = true;
		}
		//usleep(100000);

		if( test == false )
			break;
	}

	for( int i = 0 ; i < concurrent ; ++i )
	{
		busyBit[i] = 0;
		readyBit[i] = 0;
	}

	// 4. merge result
	bool hasSolved = false ,hasConflict = false;
	for( int i = 0 ; i < concurrent ; ++i ) {
		if( probeResult[i] == SOLVED )
			hasSolved = true;
		if( probeResult[i] == CONFLICT )
			hasConflict = true;
	}

	if( hasSolved ) return SOLVED;
	if( hasConflict ) return CONFLICT;

	return INCOMP;
}

void Worker::extendThread() {
	if( live_proc == 0 ) return;

	if( concurrent < live_proc ) {
		for( long i = concurrent ; i < live_proc ; ++i ) {
			int rc = pthread_create( &pool[i], NULL, run, (void*)i );
			if( rc != 0 ) {
				puts("thread create error");
				exit(1);
			}
		}

		concurrent = live_proc;
		printf("thread extend to %d\n",concurrent);
	}
}

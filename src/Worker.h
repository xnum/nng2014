#ifndef WORKER_H
#define WORKER_H

#include <cstdio>
#include <cstdlib>
#include "pthread.h"
#include "unistd.h"

#include "cdef.h"
#include "fullyprobe.h"
#include "linesolve.h"

extern int live_proc;

class Worker {
	public:
		Worker();
		~Worker();
		void extendThread();
		int setAndRun(FullyProbe* f,LineSolve* l,Board board);

	private:
		pthread_t pool[32];
		int concurrent;
};

#endif

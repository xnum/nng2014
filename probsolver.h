#ifndef PROB_SOLVER_H
#define PROB_SOLVER_H
#include "board.h"
#include "scanner.h"
#include "linesolve.h"
#include "fullyprobe.h"
#include "help.h"
#include "mirror.h"


class NonogramSolver
{
	public:
		NonogramSolver() {}
		int doSolve(int *data);
		void setMethod(int n);
		void dfs(FullyProbe&,LineSolve&,Board);	
		Board getSolvedBoard(){ return ls.solvedBoard; }
	private:
		LineSolve ls;
		FullyProbe fp;
		bool finish;
		int times;
};

#endif


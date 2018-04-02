#ifndef PROB_SOLVER_H
#define PROB_SOLVER_H

#include <deque>

#include "board.h"
#include "scanner.h"
#include "linesolve.h"
#include "fullyprobe.h"
#include "help.h"
#include "mirror.h"

using std::deque;

class NonogramSolver
{
	public:
		NonogramSolver() {}
		int doSolve(int *data);
		void setMethod(int n);
		void dfs(FullyProbe&,LineSolve&,Board);	
		void dfs_stack(FullyProbe&,LineSolve&,Board,int);
		Board getSolvedBoard(){ return ls.solvedBoard; }

		int times;
	private:
		LineSolve ls;
		FullyProbe fp;
		bool finish;
		int thres;
		int sw;
		deque<Board> queue;
    vector<Board> deprecated_boards;
		int depth_rec[626];
		int max_depth;
};

#endif


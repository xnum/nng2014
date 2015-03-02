#ifndef TEST_H
#define TEST_H

#include "board.h"
#include "fullyprobe.h"
#include <vector>

using std::vector;

struct node {
	int x,y;
};

class mirror {
    public:
		vector<Board> vec;
		vector<Board> pattern;

        mirror( FullyProbe& , Board& , size_t );
		bool generatePattern( Board& , int );
	private:
		int checkType( Board a , Board b , int x , int y );
};

#endif

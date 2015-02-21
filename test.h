#ifndef TEST_H
#define TEST_H

#include "board.h"
#include "fullyprobe.h"
#include <vector>

using std::vector;

struct node {
  int x,y;
};

class test {
 public:
  test( FullyProbe& , Board& );
  int compare( vector<node> &unk , Board a , Board b );
  int checkType( Board a , Board b , int x , int y );

  vector<Board> vec;

  int count1 , count2;
};

#endif

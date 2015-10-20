#ifndef HELP_H
#define HELP_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

#include "mirror.h"
#include "scanner.h"

using namespace std;

int* allocMem(int n);
void clearFile(const char* s);
void listPixel( FullyProbe& fp, Board &b );
void printProb( int data[] , const char* name , int probNum );
void expandInputFile( const char* name );

#endif


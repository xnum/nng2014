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

class Options
{
	public:
		int problemStart;
		int problemEnd;
		int method;
		char inputFileName[512];
		char outputFileName[512];
		char logFileName[100];
		bool selfCheck;
		bool keeplog;
		bool simple;

		Options()
		{
			problemStart = 1;
			problemEnd = 1000;
			method = CH_MUL;
			memset(inputFileName,0,sizeof(inputFileName));
			memset(outputFileName,0,sizeof(outputFileName));
			memset(logFileName,0,sizeof(logFileName));
			strcpy(inputFileName,"input.txt");
			strcpy(outputFileName,"output.txt");
			selfCheck=true;
			keeplog=false;
			simple=false;
		}

		void print()
		{
			printf("Problem Start = %d\n",problemStart);
			printf("Problem End = %d\n",problemEnd);
			printf("Method = %d\n",method);
			printf("Input File = %s\n",inputFileName);
			printf("Output File = %s\n",outputFileName);
			printf("Log File = %s\n",logFileName[0]==0 ? "log_[timestamp].txt" : logFileName );
			printf("Self Ans Check = %c\n",selfCheck==true?'Y':'N');
			printf("Write Log = %c\n",keeplog?'Y':'N');
			printf("Print Each 100 Solved = %c\n",simple?'Y':'N');
		}
};

void printUsage(const char *name);
int parseOptions(int argc, char **argv, Options &opt);
int* allocMem(int n);
void clearFile(const char* s);
void listPixel( FullyProbe& fp, Board &b );
int genLog( const Options &option, char* logName, int size );
void printProb( int data[] , const char* name , int probNum );
void expandInputFile( const char* name );

#endif


#ifndef OPTIONS_H
#define OPTIONS_H

#include "cdef.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

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
			strcpy(inputFileName,INPUT_FILE_NAME);
			strcpy(outputFileName,OUTPUT_FILE_NAME);
			selfCheck=true;
			keeplog=false;
			simple=false;
		}

		void print()
		{
			printf("Problems select from [%d] to [%d]\n",problemStart,problemEnd);
			printf("Method = [%d]\n",method);
			printf("Input file = [%s]\n",inputFileName);
			printf("Output file = [%s]\n",outputFileName);
			printf("Write log = [%s]\n",keeplog?"Yes":"No");
			if( keeplog )
				printf("\tLog file = [%s]\n",logFileName[0]==0 ? "log_<timestamp>.txt" : logFileName );
			printf("Self answer check = [%s]\n",selfCheck?"Yes":"No");
			printf("Only Print When Each 100 Solved = [%s]\n",simple?"Yes":"No");
		}
		
		int readOptions(int argc, char **argv);
		/*
		 * write log info about options and return log file name
		 */
		int genLogFile();
		void printUsage(const char *name);
};


#endif

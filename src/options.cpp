#include "options.h"

int Options::readOptions(int argc, char **argv)
{
	bool showResult = false;
	for( int i = 1 ; i < argc ; ++i )
	{
		if( i+1 < argc && (!strcmp(argv[i],"-S") || !strcmp(argv[i],"--start")) )
		{
			int n = atoi(argv[i+1]);
			problemStart = n;
			i++;
			continue;
		}

		if( i+1 < argc && (!strcmp(argv[i],"-E") || !strcmp(argv[i],"--end")) )
		{
			int n = atoi(argv[i+1]);
			problemEnd = n;
			i++;
			continue;
		}

		if( i+1 < argc && (!strcmp(argv[i],"-I") || !strcmp(argv[i],"--input")) )
		{
			strncpy(inputFileName,argv[i+1],512);
			i++;
			continue;
		}	

		if( i+1 < argc && (!strcmp(argv[i],"-O") || !strcmp(argv[i],"--output")) )
		{
			strncpy(outputFileName,argv[i+1],512);
			i++;
			continue;
		}

		if( i+1 < argc && (!strcmp(argv[i],"-L") || !strcmp(argv[i],"--log")) )
		{
			strncpy(logFileName,argv[i+1],100);
			i++;
			continue;
		}

		if( i+1 < argc && (!strcmp(argv[i],"-M") || !strcmp(argv[i],"--method")) )
		{
			int n = atoi(argv[i+1]);
			if( n<1 || n>7 ) n = CH_MUL;
			method = n;
			i++;
			continue;
		}

		if( !strcmp(argv[i],"--no-self-check") )
		{
			selfCheck=false;
			continue;
		}

		if( !strcmp(argv[i],"--yes-log") )
		{
			keeplog=true;
			continue;
		}

		if( !strcmp(argv[i],"--simple-stdout") )
		{
			simple=true;
			continue;
		}

		if( !strcmp(argv[i],"--show-config") )
		{
			showResult = true;
			continue;
		}

		// all options are not fit
		printUsage(argv[0]);
		return 0;
	}

	print(showResult);

	return 1;
}

void Options::printUsage(const char *name)
{
	printf("%s [options]\n",name);
	printf("  -S N\n");
	printf("  --start N\n");
	printf("    start from problem N\n");

	printf("  -E N\n");
	printf("  --end N\n");
	printf("    end by problem N\n");

	printf("  -I [file]\n");
	printf("  --input [file]\n");
	printf("    set input file, default:input.txt\n");

	printf("  -O [file]\n");
	printf("  --output [file]\n");
	printf("    set output file, default:output.txt\n");

	printf("  -L [file]\n");
	printf("  --log [file]\n");
	printf("    set log file name\n");

	printf("  -M n\n");
	printf("  --method n\n");
	printf("    set choosing method 1~7\n");

	printf("  --yes-log\n");
	printf("    open timer log\n"); 

	printf("  --no-self-check\n");
	printf("    don't check answer\n");

	printf("  --simple-stdout\n");
	printf("    only show total time at exit\n");
	printf("\nSample:\n%s -S 1 -E 1000 -I input.txt -O out.txt -M 4\n\n",name);

	printf("  --show-config\n");
	printf("    show parsed result of command line\n");

}


int Options::genLogFile()
{
	if( keeplog == false )
		return 0;

	if( logFileName[0] == 0 )
	{
		time_t rawtime = time(NULL);
		struct tm *timeinfo = localtime(&rawtime);
		strftime(logFileName, sizeof(logFileName) , "log_%H_%M__%b_%d_%G.txt",timeinfo);
	}

	FILE* log = fopen( logFileName , "w" );

	fprintf( log, "problem start: %d\n" ,problemStart );
	fprintf( log, "problem end: %d\n" ,problemEnd );
	fprintf( log, "dfs max times: %d\n" , DFS_MAX_TIMES );
	fprintf( log, "reduce max perm: %d\n" , MAX_PERM );
	fprintf( log, "pattern drop threshold: %d\n" , PATTERN_DROP_THRESHOLD );

	fprintf( log , "choose pixel method: ");
	switch(method) {
		case CH_SUM:
			fputs( "SUM\n" , log );
			break;
		case CH_MIN:
			fputs( "MIN\n" , log );
			break;
		case CH_MAX:
			fputs( "MAX\n" , log );
			break;
		case CH_MUL:
			fputs( "MUL\n" , log );
			break;
		case CH_SQRT:
			fputs( "SQRT\n" , log );
			break;
		case CH_MIN_LOGM:
			fputs( "MIN_LOGM\n" , log );
			break;
		case CH_MIN_LOGD:
			fputs( "MIN_LOGD\n" , log );
			break;
		default:
			break;
	}

#ifdef MIRROR
	fprintf( log , "Mirror: Enable\n");
#else
	fprintf( log , "Mirror: Disable\n");
#endif

#ifdef ZHASH
	fprintf( log , "zhash: Enable\n");
#else
	fprintf( log , "zhash: Disable\n");
#endif


#ifdef CUT_BY_SIZE
	fprintf( log , "ls size cut: Enable\n");
#else
	fprintf( log , "ls size cut: Disable\n");
#endif

#ifdef CUT_BY_CACHE
	fprintf( log , "ls cache: Enable\n");
#else
	fprintf( log , "ls cache: Disable\n");
#endif

#ifdef USE_FP2
	fprintf( log , "fp2: Enable\n");
#else
	fprintf( log , "fp2: Disable\n");
#endif


	fclose(log);

	return 0;
}



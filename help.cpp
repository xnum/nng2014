#include "help.h"

int parseOptions(int argc, char **argv, Options &opt)
{
	for( int i = 1 ; i < argc ; ++i )
	{
		if( i+1 < argc && (!strcmp(argv[i],"-S") || !strcmp(argv[i],"--start")) )
		{
			int n = atoi(argv[i+1]);
			opt.problemStart = n;
			i++;
			continue;
		}

		if( i+1 < argc && (!strcmp(argv[i],"-E") || !strcmp(argv[i],"--end")) )
		{
			int n = atoi(argv[i+1]);
			opt.problemEnd = n;
			i++;
			continue;
		}
	
		if( i+1 < argc && (!strcmp(argv[i],"-I") || !strcmp(argv[i],"--input")) )
		{
			strncpy(opt.inputFileName,argv[i+1],512);
			i++;
			continue;
		}	

		if( i+1 < argc && (!strcmp(argv[i],"-O") || !strcmp(argv[i],"--output")) )
		{
			strncpy(opt.outputFileName,argv[i+1],512);
			i++;
			continue;
		}

		if( i+1 < argc && (!strcmp(argv[i],"-L") || !strcmp(argv[i],"--log")) )
		{
			strncpy(opt.logFileName,argv[i+1],100);
			i++;
			continue;
		}

		if( i+1 < argc && (!strcmp(argv[i],"-M") || !strcmp(argv[i],"--method")) )
		{
			int n = atoi(argv[i+1]);
			if( n<1 || n>7 ) n = CH_MUL;
			opt.method = n;
			i++;
			continue;
		}

		if( !strcmp(argv[i],"--no-self-check") )
		{
			opt.selfCheck=false;
			continue;
		}

		if( !strcmp(argv[i],"--yes-log") )
		{
			opt.keeplog=true;
			continue;
		}

		if( !strcmp(argv[i],"--simple-stdout") )
		{
			opt.simple=true;
			continue;
		}


		printUsage(argv[0]);
		return 1;
	}

	return 0;
}

void printUsage(const char *name)
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

}

int* allocMem(int n)
{
    int* ptr = new int[n];
    testFail( ptr!=NULL , "alloc input data mem  error");

    return ptr;
}

void clearFile(const char* s)
{
    FILE* out = fopen( s ,"w" );
    testFail( out!=NULL , "open output.txt error");
    fclose(out);
}

void listPixel( FullyProbe& fp, Board &b )
{
    vector<node> unk;

    Dual_for(i,j)
        if( getBit( b , i , j ) == BIT_UNKNOWN )
        {
            node tmp;
            tmp.x = i;
            tmp.y = j;
            unk.push_back( tmp );
        }

    printf("%zu\n" , unk.size() );

    printf("[xxxxxx]        ");
    Dual_for(i,j)
        if( getBit( b , i , j ) == BIT_UNKNOWN )
            printf("%4zu ",i);
    puts("");

    printf("[xxxxxx]        ");
    Dual_for(i,j)
        if( getBit( b , i , j ) == BIT_UNKNOWN )
            printf("%4zu ",j);
    puts("");


    for( size_t i = 0 ; i < unk.size() ; ++i )
    {
        int unkx = unk[i].x;
        int unky = unk[i].y;
        for( int v = 0 ; v < 2 ; ++v )
        {
            Board &gp = fp.gp[unkx][unky][v];
            printf("[%d,%d,%d]\t" , unkx , unky , v );
            for( size_t k = 0 ; k < unk.size() ; ++k )
            {
                int x = unk[k].x;
                int y = unk[k].y;
                printf("    %c" , getBit( gp , x , y )==BIT_ZERO ? '0' : ( getBit( gp , x , y )==BIT_ONE?'1':'u' )  );
            }
            putchar('\n');
        }
    }

}

int genLog( const Options &option, char* logName, int size )
{
	if( logName[0] == 0 )
	{
		time_t rawtime = time(NULL);
		struct tm *timeinfo = localtime(&rawtime);
		strftime(logName, size, "log_%H_%M__%b_%d_%G.txt",timeinfo);
	}

    FILE* log = fopen( logName , "w" );

    fprintf( log, "problem start: %d\n" ,option.problemStart );
    fprintf( log, "problem end: %d\n" ,option.problemEnd );
    fprintf( log, "dfs max times: %d\n" , DFS_MAX_TIMES );
    fprintf( log, "reduce max perm: %d\n" , MAX_PERM );
    fprintf( log, "pattern drop threshold: %d\n" , PATTERN_DROP_THRESHOLD );

    fprintf( log , "choose pixel method: ");
    switch(option.method) {
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

void printProb( int data[] , const char* name , int probNum )
{
	FILE* file;

	if(probNum==1)
		file = fopen(name,"w");
	else
		file = fopen(name,"a+");

	fprintf(file,"$%d\n",probNum);
	for( int i = 0 ; i < 50 ; ++i )
		for( int j = 1 ; j <= data[i*14] ; ++j )
			fprintf(file ,"%d%c" , data[i*14+j] , 
					j==data[i*14] ? '\n' : '\t' );

	fclose(file);
}

void expandInputFile( const char* name )
{
	fstream fin( name , ios::in );
	
	string line;
	vector<string> input;
	while( getline( fin , line ) )
		for( int i = 0 ; i < 50 ; ++i )
		{
			getline( fin , line );
			input.push_back( line );	
		}

	const int bound = input.size() / 50;

	fstream fout( name , ios::out );

	srand(time(NULL));

	for( int i = 0 ; i < 1000 ; ++i )
	{
		int prob = i % bound;
		fout << "$" << i+1 << endl;			
		for( int j = 0 ; j < 50 ; ++j )
			fout << input[prob*50+j] << endl;
	}
	
}



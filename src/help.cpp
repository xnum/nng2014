#include "help.h"
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



#include "help.h"


int MPI_MyRecv(void *buff, int count, MPI_Datatype datatype, 
        int from, int tag, MPI_Comm comm, MPI_Status *status) {

    int flag, nsec_start=1000, nsec_max=100000;
    struct timespec ts;
    MPI_Request req;

    ts.tv_sec = 0;
    ts.tv_nsec = nsec_start;

    PMPI_Irecv(buff, count, datatype, from, tag, comm, &req);
    do {
        nanosleep(&ts, NULL);
        ts.tv_nsec *= 2;
        ts.tv_nsec = (ts.tv_nsec > nsec_max) ? nsec_max : ts.tv_nsec;
        PMPI_Request_get_status(req, &flag, status);
    } while (!flag);

    return (*status).MPI_ERROR;
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

void runAsMaster(const Options& option,int size,Board answer[])
{
    int doneNum = 0;
    int probN = option.problemStart;

    int sentSlave = 0;
    while( 1 )
    {
        if( probN > option.problemEnd )
        {
            probN = DONE;
            if( sentSlave == size-1 && doneNum == option.problemEnd-option.problemStart+1 )
                break;
        }

        MPI_Status status;
        Board b;
        MPI_MyRecv(b.data ,50 ,MPI_UNSIGNED_LONG_LONG ,MPI_ANY_SOURCE ,MPI_ANY_TAG ,MPI_COMM_WORLD ,&status);
        if( status.MPI_TAG == 0 )  // ask for a new problem
        {
            if( probN == DONE )
                sentSlave++;
            MPI_Send(b.data ,50 ,MPI_UNSIGNED_LONG_LONG ,status.MPI_SOURCE ,probN ,MPI_COMM_WORLD);
            probN++;
        }
        else // retrieve a solved board
        {
            if( 1 <= status.MPI_TAG && status.MPI_TAG <= 1000 ) // for a legal tag
            { // check answer
                answer[status.MPI_TAG] = b;
                doneNum++;
            }
            else
            {
                fprintf(stderr,"Illegal TAG = %d\n",status.MPI_TAG);
            }
        }
    }
}

int RecvFromMaster()
{
    MPI_Status status;
    Board b;
    MPI_Send(b.data ,50 ,MPI_UNSIGNED_LONG_LONG ,0 ,0 ,MPI_COMM_WORLD);
    MPI_Recv(b.data ,50 ,MPI_UNSIGNED_LONG_LONG ,0 ,MPI_ANY_TAG ,MPI_COMM_WORLD ,&status);
    int probN = status.MPI_TAG;
    if(probN == DONE)
        return -1;

    return probN;
}

void sendToMaster(Board ans,int probN)
{
    MPI_Send(ans.data ,50 ,MPI_UNSIGNED_LONG_LONG ,0 ,probN ,MPI_COMM_WORLD);
}

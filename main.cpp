/*
 * backtrack version
 *
 *
 *
 *
 *
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "test.h"
#include "board.h"
#include "scanner.h"
#include "linesolve.h"
#include "fullyprobe.h"
#include "help.h"

using namespace std;

FullyProbe fp;
int* inputData;
int probData[50*14];
bool finish = false;
int times = 0;

int prevent[25][25][2] = {};
int Org_unkP[25][25] = {};


void dfs( FullyProbe& fp , LineSolve& ls , Board b )
{
    if( times == DFS_MAX_TIMES )
    {
        finish = true;
        return;
    }

    times++;

    int res = fp2( fp , ls , b );
    if( res == SOLVED )
    {
        finish = true;
        return;
    }

    if( res == CONFLICT )
        return;

    Board b1 = fp.max_g1;
    Board b0 = fp.max_g0;

    dfs( fp , ls , b0 );

    if( finish == true )
        return;

    dfs( fp , ls , b1 );
}

int apply( Board& G , Board &gp , Board &org_b , size_t x , size_t y , int c , int unk[25][25] );

int applyHelper( LineSolve &ls , Board& b )
{

    MEMSET_ZERO(prevent);
    int unkP[25][25] = {};

    Dual_for(i,j)
        if( getBit( b , i , j ) == BIT_UNKNOWN )
            Org_unkP[i][j] = unkP[i][j] = 1;
        else
            Org_unkP[i][j] = unkP[i][j] = 0;

    Board Org_b = b;

    int maxX = 0 , maxY = 0 ,maxV = 0 , maxVal = 0;
    Dual_for(x,y)
        if( unkP[x][y] == 1 )
            for( int z = 0 ; z < 2 ; ++z )
            {
                int size = getSize(fp.gp[x][y][z]);
                if( maxVal < size )
                {
                    maxVal = size;
                    maxX = x;
                    maxY = y;
                    maxV = z;
                }
            }

    Board Gp0 = fp.gp[maxX][maxY][maxV];
    Board Gp1 = fp.gp[maxX][maxY][maxV==0?1:0];

    Board b1 = b , b0 = b;

    if( CONFLICT != apply(b0,Gp0,Org_b,maxX,maxY,maxV,unkP)) 
    {
        if( finish == true )
        {
            if( checkAns(b0,probData) )
                printBoard(b0,ls.probN);
            else
            {
                puts("ERROR");
                exit(1);
            }
            return SOLVED;
        }
        if( SOLVED == applyHelper( ls , b0 ) )
            return SOLVED;
    }

    if( CONFLICT != apply(b1,Gp1,Org_b,maxX,maxY,maxV==0?1:0,unkP))
    {
        if( finish == true )
        {
            if( checkAns(b1,probData) )
                printBoard(b1,ls.probN);
            else
            {
                puts("ERROR");
                exit(1);
            }
            return SOLVED;
        }
        if( SOLVED == applyHelper( ls , b1 ) )
            return SOLVED;
    }

    return CONFLICT;
}

int apply( Board& G , Board &gp , Board &org_b , size_t x , size_t y , int c , int unk[25][25] )
{
    int applied[25][25][2]={};

    if( getBit( G , x , y ) != BIT_UNKNOWN && getBit( G , x , y ) != c+BIT_ZERO )
    {
        return CONFLICT;
    }

    if( prevent[x][y][c] == 1 )
        return 5566;


    prevent[x][y][c] = 1;

    int set[25][25] = {};


    Dual_for(i,j)
    {
        if( getBit( org_b , i , j ) == BIT_UNKNOWN && getBit( gp , i , j ) != BIT_UNKNOWN )
        {
            if( getBit( G , i , j ) == BIT_UNKNOWN )
            {
                set[i][j] = getBit( gp , i , j );
            } 
            else if ( getBit( G , i , j ) != getBit( gp , i , j ) )
            {
                prevent[x][y][c] = 0;
                return CONFLICT;
            }
        }
    }

    Board Orig_G = G;
    for( int k = 0 ; k < 50 ; ++k )
        G.data[k] &= gp.data[k];

    Dual_for(i,j)
    {
        if( set[i][j] )
        {
            if( CONFLICT == apply( G , fp.gp[i][j][set[i][j]-BIT_ZERO] , org_b, i , j , set[i][j]-BIT_ZERO , unk ) )
            {
                Dual_for(k,l)
                    for (int m = 0; m < 2; m++)
                        if (applied[k][l][m] == 1)
                        {
                            unk[k][l] = 1;
                            prevent[k][l][m] = 0;
                        }
                G = Orig_G;
                prevent[x][y][c] = 0;
                return CONFLICT;
            }
            else
            {
                applied[i][j][set[i][j]-BIT_ZERO] = 1;
                unk[i][j] = 0;
            }
        }
    }

    Dual_for(i,j)
    {
        if( x==i && y==j )
            continue;
        int bit_c = c==0 ? BIT_ZERO : BIT_ONE;
        if( Org_unkP[i][j] )
        {
            if( getBit( fp.gp[i][j][0] , x , y ) == getBit( fp.gp[i][j][1] , x , y ) )
            {
                if( getBit( fp.gp[i][j][0] , x , y ) == BIT_UNKNOWN )
                    continue;
                if( getBit( fp.gp[i][j][0] , x , y ) != bit_c )
                {
                    Dual_for(k,l)
                        for (int m = 0; m < 2; m++)
                            if (applied[k][l][m] == 1)
                            {
                                unk[k][l] = 1;
                                prevent[k][l][m] = 0;
                            }
                    G = Orig_G;
                    prevent[x][y][c]  = 0;
                    return CONFLICT;
                }
            }
            else // 2 sides not same
            {
                if( getBit( fp.gp[i][j][0] , x , y ) == BIT_UNKNOWN )
                {
                    if( getBit( fp.gp[i][j][1] , x , y ) != bit_c )
                    {
                        if( CONFLICT == apply( G ,fp.gp[i][j][0] ,org_b,i ,j ,0 ,unk ))
                        {
                            Dual_for(k,l)
                                for (int m = 0; m < 2; m++)
                                    if (applied[k][l][m] == 1)
                                    {
                                        unk[k][l] = 1;
                                        prevent[k][l][m] = 0;
                                    }
                            G = Orig_G;
                            prevent[x][y][c] = 0;
                            return CONFLICT;
                        }
                        else
                        {
                            applied[i][j][set[i][j]-BIT_ZERO] = 1;
                            unk[i][j] = 0;
                        } 
                    }
                }
                else if( getBit( fp.gp[i][j][1] , x , y ) == BIT_UNKNOWN )
                {
                    if( getBit( fp.gp[i][j][0] , x , y ) != bit_c )
                    {
                        if( CONFLICT == apply( G ,fp.gp[i][j][1] ,org_b,i ,j ,1 , unk ))
                        {
                            Dual_for(k,l)
                                for (int m = 0; m < 2; m++)
                                    if (applied[k][l][m] == 1)
                                    {
                                        unk[k][l] = 1;
                                        prevent[k][l][m] = 0;
                                    }
                            G = Orig_G;
                            prevent[x][y][c] = 0;
                            return CONFLICT;
                        }
                        else
                        {
                            applied[i][j][set[i][j]-BIT_ZERO] = 1;
                            unk[i][j] = 0;
                        } 
                    }
                }
                else if( getBit( fp.gp[i][j][1] , x , y ) != bit_c )
                {
                    if( CONFLICT == apply( G ,fp.gp[i][j][0] ,org_b,i ,j ,0 ,unk ) )
                    {
                        Dual_for(k,l)
                            for (int m = 0; m < 2; m++)
                                if (applied[k][l][m] == 1)
                                {
                                    unk[k][l] = 1;
                                    prevent[k][l][m] = 0;
                                }
                        G = Orig_G;
                        prevent[x][y][c] = 0;
                        return CONFLICT;
                    }
                    else
                    {
                        applied[i][j][set[i][j]-BIT_ZERO] = 1;
                        unk[i][j] = 0;
                    } 
                }
                else
                {
                    if( CONFLICT == apply( G ,fp.gp[i][j][1] ,org_b,i ,j ,1 ,unk ) )
                    {
                        Dual_for(k,l)
                            for (int m = 0; m < 2; m++)
                                if (applied[k][l][m] == 1)
                                {
                                    unk[k][l] = 1;
                                    prevent[k][l][m] = 0;
                                }
                        G = Orig_G;
                        prevent[x][y][c] = 0;
                        return CONFLICT;
                    }
                    else
                    { 
                        applied[i][j][set[i][j]-BIT_ZERO] = 1;
                        unk[i][j] = 0;
                    } 
                }
            }
        }
    }
    finish = true;

    Dual_for(ii,jj)
        if( getBit( G,ii,jj)==BIT_UNKNOWN )
        {
            finish = false;
            ii=jj=1000;
        }

    if( finish == true )
    {
        if( !checkAns( G , probData ) )
        {
            printf("ERROR");
            exit(1);
        }
    }

    return SOLVED;

}



int main(int argc , char *argv[])
{

    int method=CH_MUL;
    if( argc==2 )
        method = argv[1][0]-'0';
    fp.method = method;

    char logName[50] = {};
    sprintf( logName ,"log_sorted_P%d_T%d.txt" ,MAX_PERM ,PATTERN_DROP_THRESHOLD );
    printLog(logName, method);

    clearFile("output.txt");

    inputData = allocMem(1001*50*14);
    readFile(inputData);

    time_t start_time = time(NULL);
    clock_t start;

    LineSolve ls;

    for( int probN = PROBLEM_START ; probN <= PROBLEM_END ; ++probN )
    {
        start = clock();

        getData( inputData ,probN ,probData );
        ls.load(probData,probN);

        Board b;

        if( SOLVED != fp2( fp , ls , b ) )
        {
            finish = false;
            times = 0;
            dfs( fp , ls , b );  
            //applyHelper( ls, b);

        }

        printf ( "$%3d\ttime:%3.4fs\ttotal:%4lds\t%6d\n" , probN
                , ( double ) ( clock() - start ) / CLOCKS_PER_SEC
                , time ( NULL ) - start_time , times
               );

        FILE* log = fopen( logName , "a+" );
        fprintf ( log , "$%3d\ttime:%3.4fs\ttotal:%4lds\t%6d\n" , probN
                , ( double ) ( clock() - start ) / CLOCKS_PER_SEC
                , time ( NULL ) - start_time , times
                );
        fclose(log);
    }

    delete[] inputData;

    return 0;
}



#include "scanner.h"

void readFile(int* data)
{
    FILE *ptr = fopen ( "input.txt" , "r" );
    testFail( ptr != NULL , "open input.txt error" );

    for( int probNum = 0 ; probNum < 1000 ; ++probNum )
    {
        char buff[100] = {};
        testFail( NULL != fgets( buff , 100 , ptr ) , "fgets error");

        for ( int i = 0 ; i < 50 ; ++i )
        {
            int n = 0 , count = 1;
            char c = 0;

            char buff2[200] = {};
            char* g = fgets( buff2 , 200 , ptr );
            int offset=0;
            while ( sscanf ( g , " %d%c%n" , &n , &c , &offset ) )
            {
                g += offset;

                data[ probNum * 50 * 14 + i * 14 + count ] = n;

                if ( c == 10 || c == 13 )
                {
                    data[ probNum * 50 * 14 + i * 14 ] = count;
                    break;
                }

                count++;
            }
        }
    }

    fclose(ptr);
}

void getData ( int* _data , int probNum , int* dest )
{
    probNum--;

    memcpy( dest , _data+50*14*probNum , sizeof(int)*50*14 );

    return ;
}

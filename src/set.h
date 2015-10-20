#ifndef SET_H
#define SET_H
#include <stdint.h>

class myset {
    public:
        myset()
        {
            clear();
        }

        inline int begin()
        {
            for( int i = 0 ; i < 10 ; ++i )
            {
                if( arr[i] )
                {
                    int x = 0;
                    x = __builtin_ffsll(arr[i]);
                    arr[i] &= arr[i]-1;
                    return x+i*64-1;
                }
            }
            return -1;
        }

        inline void insert(int n)
        {
            arr[n/64] |= 1LL << (n%64);
        }

        inline void remove(int n)
        {
            arr[n/64] &= ~1LL << (n%64);
        }

        inline void clear()
        {
            for( int i = 0 ; i < 10 ; ++i )
            {
                arr[i] = 0LL;
            }
        }

		inline bool isEmpty()
		{
			for( int i = 0 ; i < 10 ; ++i )
				if( arr[i] != 0 )
					return false;
			return true;
		}

    private:
        uint64_t arr[10];
};

#endif

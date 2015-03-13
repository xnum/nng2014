#include "bcache.h"

int canUseBigCache()
{
	fstream fin( INPUT_FILE_NAME , ios::in );
	map<string,int> counter;
	string line;
	while(getline(fin,line))
		counter[line]++;

	if( counter.size() < 16384 )
		return 1;

	return 0;
}

BigCache::BigCache()
{
	fstream fin( INPUT_FILE_NAME , ios::in );

	map<string,int> counter;
	vector<string> probs;
	string line;
	while(getline(fin,line))
	{
		if( line[0] != '$' )
			probs.push_back(line);
	}

	vector<string> up(probs);

	sort( up.begin() , up.end() );

	auto it = unique( up.begin() , up.end() );

	up.resize( distance(up.begin(),it) );
	cout << probs.size() << endl;
	
	for( size_t i = 0 ; i < probs.size() ; ++i )
	{
		size_t index = 0;
		for( ; index < up.size() ; ++index )
			if( up[index] == probs[i] )
				break;
		if( index == up.size() )
			exit(111);
		keyTable[i] = index;
	}

	curr_used = 0;

#ifdef ZHASH
	pool = (uint64_t*)malloc(sizeof(uint64_t)*1024*1024*80);
	if( pool == NULL )
	{
		puts("malloc error");
		exit(13);
	}
	hash = zhash_new();
	pool_ptr = 0;
#endif

	cout << "Init done" << endl;
}

BigCache::~BigCache()
{
#ifdef ZHASH
	zhash_destroy(&hash);
	free(pool);
#endif
}

void BigCache::insert(int prob,int line,uint64_t key,uint64_t val)
{
	if( curr_used > 600L*1024L*1024L )
		clear();
	curr_used += sizeof(uint64_t)*8;
#ifdef ZHASH
	if( pool_ptr+1 >=1024*1024*80 )
		clear();
	pool[pool_ptr] = val;
	uint64_t newkey = encrypt(prob,line,key);
	zhash_insert( hash , (char*)&newkey , &pool[pool_ptr] );
	pool_ptr++;
#else
	table[encrypt(prob,line,key)] = val;
#endif
}

uint64_t BigCache::query(int prob,int line,uint64_t key)
{
#ifdef ZHASH
	uint64_t newkey = encrypt(prob,line,key);
    uint64_t* res = (uint64_t*)zhash_lookup( hash , (char*)&newkey );
    if( res == NULL )
        return NOT_FOUND;
    else
        return *res;
#else
	auto it = table.find(encrypt(prob,line,key));
	if( it == table.end() )
	{
		return NOT_FOUND;
	}
	else
	{
		return it->second;
	}
#endif
}



void BigCache::clear()
{
	curr_used = 0;
#ifdef ZHASH
	pool_ptr = 0;
#else
	table.clear();
#endif
}



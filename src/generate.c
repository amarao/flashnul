#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define MAX_ALLOWED_BLOCK 1048576
/**
	This module contain three functions:
		create_filled_block
			creating a block of memory filled by specified byte
			(block must be freed after use)

		create_incremental_block
			creating a block of incrementing bytes.

**/


unsigned char* create_filled_block( long long block_size, unsigned char filler ){

	unsigned char* retval;

	if( block_size > MAX_ALLOWED_BLOCK )
		exit( printf ("requested block (%I64d bytes) is too large\n", block_size ) );

	retval = malloc( (size_t)block_size );
	assert( retval );

	memset( retval, filler, block_size );

	return retval;

}

unsigned char* create_incremental_block( long long offset,int sector_size, long long size,long long iterator ){

	int initial_value;
	int c;
	unsigned char* retval;

	assert( size % sizeof(size_t) == 0 );

	retval = malloc( (size_t)size );
   	assert( retval );

	initial_value = (offset+iterator)/sizeof(int);

	for( c = 0; c < size / sizeof( int ); c++ )
		((int*)retval)[c] = initial_value + c;

	return retval;

}
			
unsigned char* create_chess_block( long long offset,int sector_size, long long size, int counter){
	char *pattern1="\xFF\x0";
	char *pattern2="\x0\xFF";
	int c;
	unsigned char* retval = malloc( (size_t)size );

	assert( retval );

	if(size%2){
		exit(printf("sorry, test pattern do not fit in block size (%I64d)\n", size));
	}
	for(c = 0; c< size;c+=2){
		if( counter % 2 )
			memcpy(retval+c,pattern1,2);
		else
			memcpy(retval+c,pattern2,2);
	}
	
	return retval;
}

unsigned char* create_test_block( long long offset,int sector_size, long long size, int counter, int mode ){

	if( size > MAX_ALLOWED_BLOCK )
		exit( printf ("error: requested block (%I64d bytes) is too large\n", size ) );

	if( !size )
		exit( printf( "error: requested a block with size 0 bytes\n"));	

	switch (mode){
		case 0:
			return create_incremental_block( offset, sector_size, size, counter*size);
		case 1:
			return create_chess_block( offset, sector_size, size, counter);
		default:
			printf("Sorry, unsupported mode (-m=%d)\n",mode);
			exit(0);
	}
	return NULL;
}



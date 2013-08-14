#ifndef _H_GENERATE_H
#define _H_GENERATE_H
unsigned char* create_filled_block( long long block_size, unsigned char filler );
unsigned char* create_test_block( long long offset,int sector_size, long long size, int counter, int mode );
unsigned char* create_incremental_block( long long offset,int sector_size, long long size, long long iterator );
#endif

#ifndef _H_FLASHNUL_H
#define _H_FLASHNUL_H
#include <windows.h>
#include <stdio.h>

typedef struct{
	long long start;
	long long end;
	int ignore_error;
	int verify_mode;
	int block_size;
	int mode;
	int delay;
	int write_share;
	FILE* log;
}io_ctrl_t;

#endif

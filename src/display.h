#ifndef _H_DISPLAY_H
#define _H_DISPLAY_H
#include <stdarg.h>
#include <stdio.h>
#include "io.h"
#include "flashnul.h"
char* human_view( const long long size, const char postletter );
long long delta(long long size);
void print_bit_dump( unsigned char* one, unsigned char* two, long long size, long long offset, int max_diff);
void wait_for_key(char* text);
char* human_view( const long long size, const char postletter );
void display_drive_info(dev_ctrl_t* dev);
void clearline();
enum{
        MSG_LOGABLE,
	MSG_DISPLAY_ONLY,
	MSG_CRASH
};
void print_header( char* text );
void print_footer(dev_ctrl_t* dev);
#endif

#ifndef _H_COMMON_H
#define _H_COMMON_H

long long compare_mem(unsigned char *one, unsigned char *two, long long size);
long long get_near_round_value(long long size);
long long delta(long long size);
char* human_view( const long long size, const char postletter );
void bitfield(char* buff,int value);

/* Somehow safe for overflow vesion of MULDIV(A,B,C) =A*B/C   */


//#define MULDIV(A,B,C) ((A*B>A)?((A*B)/C):((max(A,B)/C)*min(A,B)))
/*Interger (not very good)*/

#define MULDIV(A,B,C) ((long long)( (double)A* (double)B / (double)C ))
/*float point version*/




#endif

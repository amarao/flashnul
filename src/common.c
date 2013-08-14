/* non-io functions */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

long long compare_mem(unsigned char *one, unsigned char *two, long long size){
	long long diff_counter=0;
	long long counter=0;
	while(counter<size){
		if(one[counter]!=two[counter]){
			diff_counter++;
		}	
		counter++;
	}
	return diff_counter;
}


long long get_near_round_value(long long size){
    int c=0;
    long long up=1;
    long long down=1;
	long long temp=size;
	while(temp!=0){
		temp=temp>>1;
		c++;
	}
	up=up<<c;
	down=down<<(c-1);
	if(size-down>size-up)
		return up;
	else
		return down;
}

long long delta(long long size){
	long long pow;
	pow=get_near_round_value(size);
	if(pow>size)
		return pow-size;
	else
		return size-pow;
}

char* human_view( const long long size, const char postletter ){
    long long divider;
	char postfix;
	static char ret_buffer[255];
	
	if(size<0){
		return " -not avaible- ";
	}
	
	if(size>10*(long long)1024*(long long)1024*(long long)1024){
		postfix='G';
		divider=1024*1024*1024;
	}else{
		if( size>10485760 ){
			postfix='M';
			divider=1024*1024;
		}else{
			if( size>10240 ){
				postfix='k';
				divider=1024;
			}
			else{
				postfix=postletter;
				divider=1;
			}
		}
	}

	snprintf(ret_buffer,255,"%I64i %c%c",size/divider,postfix, postfix==postletter?0:postletter);
	return ret_buffer;
}

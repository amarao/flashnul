#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include "display.h"
#include "common.h"
#include "finetune.h"

void wait_for_key(char* text){
	fprintf(stderr,text);
	getchar();
}


void bitfield(char* buff,int value){
	int c;
	assert(value<256&&value>=0);
	for(c=7;c>=0;c--){
		if(value & (1<<c))
			buff[c]='1';
		else
			buff[c]='0';
	}
	buff[8]=0;
}

void print_bit_dump( unsigned char* one, unsigned char* two, long long size, long long offset, int max_diff){
	long long counter=0;
	char one_buff[9];
	char two_buff[9];
	char diff_buff[9];
	int diff_counter=0;
	printf("Offset (hex)\tWrite\t\t  Read\t\t     XOR\n");
	while(counter<size){
		if(one[counter]!=two[counter]){
			if(diff_counter>=max_diff)
				return;
			diff_counter++;
			bitfield(one_buff,one[counter]);
			bitfield(two_buff,two[counter]);
			bitfield(diff_buff,one[counter]^two[counter]);
			printf("0x%010I64x:\t%s (0x%02X) | %s (0x%02X) | %s\n", offset + counter, one_buff,one[counter],two_buff,two[counter],diff_buff);
		}
		counter++;

	}

}



void display_drive_info(dev_ctrl_t* dev){
	printf( "\n\nDisk %s (UNC name: %s)\n", dev->user_name, dev->system_name );	

	if( dev->drive_geometry_flag ){

		print_header( "[Drive geometry]");
 		printf(	"Cylinders/heads/sectors\t = %I64d/%I64d/%I64d\n", dev->cylinders,dev->tracks_per_cylinder,dev->sectors_per_track );
		printf( "Bytes per sector\t = %I64d\n", dev->sector_size );
		printf( "CHS size\t\t = %I64d (%s)\n", dev->geometry_size, human_view( dev->geometry_size, 'b' ) );

	}

	if( dev->disk_space_flag ){

		print_header( "[Free Space]");
		printf( "Total space\t\t = %I64d\n", dev->disk_size );
		printf( "Free  space\t\t = %I64d\n", dev->free_size );
		if( dev->free_size != dev->avaible_size )
			printf( "Avaible space\t\t = %I64d (diff: %I64d)\n", dev->avaible_size, dev->avaible_size - dev->free_size );

	}

	if( dev->drive_length_flag ){
		print_header("[Device size]" );
    		printf("Device size\t\t = %I64d (%s)\n",dev->device_size,human_view(dev->device_size, 'b'));
        	printf("delta to near power of 2 = %I64d (%s), %I64d%%\n", delta(dev->device_size), human_view(delta(dev->device_size),'b'),(100*delta(dev->device_size))/dev->device_size);
		if( dev->physical_flag )printf("Surplus size\t\t = %I64d (%s)\n",dev->device_size-dev->geometry_size,human_view(dev->device_size-dev->geometry_size, 'b'));
		
	}
	if( dev->adapter_flag || dev->device_flag )
		print_header( "[Adapter & Device properties]" );

	if( dev->adapter_flag )
		printf( "Bus type\t\t = (%d) %s\n",dev->bus_type, dev->bus_name );

	if( dev->device_flag ){

		printf( "Removable device\t = %s\n", dev->removable ? "Yes" : "No" );
		printf( "Command Queue\t\t = %s\n", dev->CQ ? "Supported" : "Unsupported" );
		if( dev->vendor[0] ) 
			printf( "Device vendor\t\t = %s\n", dev->vendor );
		if( dev->name[0] )
			printf( "Device name\t\t = %s\n", dev->name );
		if( dev->revision[0] )
			printf( "Revision\t\t = %s\n", dev->revision );
		if( dev->serial[0] )
			printf( "Device serial\t\t = %s\n", dev->serial );
//		if( dev->raw_serial[0] )
//			printf( "Device raw serial= %s\n",dev->raw_serial );

	}

	if (dev->hotplug_info_flag ){
		print_header( "[Hotplug info]" );
		printf( "Device hotplug\t\t = %s\n", dev->device_hotplug ? "Yes" : "No" );
		printf( "Media hotplug\t\t = %s\n", dev->media_hotplug ? "Yes" : "No" );
	}

}


void print_header( char* text ){

	char	buffer[LINE_LENGTH+1];
	int	len = strlen(text);

	assert( len < LINE_LENGTH - 2 );
	
	memset( buffer, '-', LINE_LENGTH - len - 2 );
	memcpy( buffer + LINE_LENGTH - 2 - len, text, len );
	memcpy( buffer + LINE_LENGTH - 2, "--", 3 );
	puts( buffer );		

}


void print_footer(dev_ctrl_t* dev){
        printf("\n");
	print_header( "[Operation result]" );
	printf( "passes:\t\t\t%I64d\n", dev->pass_count  );
	printf( "errors:\t\t\t%I64d\n", dev->error_count  );
	if(!dev->size)
		printf("warining:\t\tdevice size unknown, all test skipped\n");
	if( dev->read_bytes )
		printf( "read bytes:\t\t%I64d (%s)\n", dev->read_bytes, human_view( dev->read_bytes, 'b' ) );
	if(dev->read_counts){
		printf( "avg. read speed:\t%I64d (%s/s)\n",
			MULDIV (dev->read_bytes, dev->freq, dev->read_counts ), 
			human_view( MULDIV( dev->read_bytes, dev->freq, dev->read_counts ), 'b' )
		);		
		printf("max/min read speed:\t%I64d (%s/s) / ", dev->max_read_speed, human_view( dev->max_read_speed, 'b' ) );
		printf("%I64d (%s/s)\n", dev->min_read_speed, human_view( dev->min_read_speed, 'b' ) );
	}
	if( dev->write_bytes )
		printf( "write bytes:\t\t%I64d (%s)\n", dev->write_bytes, human_view( dev->write_bytes, 'b' ) );
	if( dev->write_counts ){
		printf( "avg. write speed:\t%I64d (%s/s)\n",
			MULDIV( dev->write_bytes, dev->freq, dev->write_counts ), 
			human_view( MULDIV (dev->write_bytes, dev->freq, dev->write_counts), 'b' )
		);		
		printf("max/min write speed:\t%I64d (%s/s) / ", dev->max_write_speed, human_view( dev->max_write_speed, 'b' ) );
		printf("%I64d (%s/s)\n", dev->min_write_speed, human_view( dev->min_write_speed, 'b' ) );
	}	
}

void clearline(){
 	char* spaceline=_alloca(LINE_LENGTH+2);
 	memset(spaceline+1,' ',LINE_LENGTH);
 	spaceline[0]='\r';
 	spaceline[LINE_LENGTH]=0;
 	fprintf(stderr,spaceline);
}

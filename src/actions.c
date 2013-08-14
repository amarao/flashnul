/*actions (-F, -I, etc)*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "io.h"
#include "generate.h"
#include "display.h"
#include "colian/colian.h"
#include "flashnul.h"
#include "actions.h"
#include "common.h"

#define DUMP_SIZE 16

int compare_blocks( unsigned char* one, unsigned char* two, long long size, long long offset ){
	
	long long diff_counter=0;
	if( !size )
		return 0;

	diff_counter=compare_mem(one,two,size);
	
	if( !diff_counter )
       		return 1;
	else{
		printf("\nERROR: verification failed (read/write data mismatch)\t\t \n");
		printf("errors: %I64d, offset: 0x%I64x (%I64d), block size: %I64d\n",diff_counter,offset, offset, size);
		if(diff_counter<DUMP_SIZE)
			print_bit_dump(one,two,size,offset,diff_counter);
		else{
			printf("First %d changed bytes (of %I64d):\n",DUMP_SIZE, diff_counter);
			print_bit_dump(one,two,size,offset,DUMP_SIZE);
		}
	}
	return 0;
		
}

long long get_next_block_size( long long offset, long long size, int prefere_size, long long end ){
	#define DEFAULT_BLOCK_SIZE 65536
	long long end_value;

	if( end && ( end < size ) )
		end_value = end;
	else
		end_value = size;
	if( offset >= end_value ) 
		return 0;
	if( prefere_size )
		if( offset+prefere_size < end_value )
			return prefere_size;
	if( offset+DEFAULT_BLOCK_SIZE < end_value )
		return DEFAULT_BLOCK_SIZE;

	return end_value-offset;

}


void blank( dev_ctrl_t* dev, io_ctrl_t* io_ctrl ){

	unsigned char*	bulk	= NULL;
	unsigned char*	res_bulk= NULL;
	int		result	= 0;
	long long	offset	= io_ctrl->start;

	dev->block_size = dev->sector_size;
	bulk = create_filled_block( dev->sector_size, 0 );
	assert( bulk );

	result = write_block( dev, offset, bulk, dev->sector_size );
	if( io_ctrl->ignore_error ){
		read_block( dev, offset, &res_bulk, dev->sector_size );
		if(!compare_blocks(bulk, res_bulk,  dev->sector_size, offset ))
			dev->error_count++;

	}else{

		if( result ){
			printf( "\rWriting first sector   - ok\n" );

			result = read_block( dev, offset, &res_bulk, dev->sector_size );
			if( result ){
				printf( "\rReading first sector   - ok\n") ;

				if( !compare_blocks (  bulk,res_bulk, dev->sector_size, offset )){
					dev->error_count++;
					exit( -1 );
				}else
					printf( "\rVerifying first sector - ok\n" );

			}

		}else
			exit( -1 );

	}

	free( bulk );
	free( res_bulk );
	dev->pass_count++;

}

void msg_sleep(int time){
        clearline();
	fprintf(stderr,"\rwaiting %d sec (Ctrl-C to cancel)",time);
	Sleep(time*1000);
	fprintf(stderr,"\r                                         ");
}

void fill( dev_ctrl_t* dev, int byte, io_ctrl_t* io_ctrl ){

   	long long 	   offset	  = io_ctrl->start;

	long long 	   block_size = 0;
	unsigned char* block	  = 0;
	unsigned char* res_block  = NULL;
	static int 	   counter	  = 0;
	int 		   result	  = 0;


	block_size	= get_next_block_size( offset, dev->size, io_ctrl->block_size, io_ctrl->end );
	dev->block_size = block_size;
	block 		= create_filled_block( block_size, (unsigned char) byte );
	assert( block );

	res_block = NULL;

	if( !block_size )
		printf("Device size unknown: test skipped\n");


	while( block_size ){

		result = write_block(dev, offset, block , block_size );

		if( !result && !io_ctrl->ignore_error )
			exit( -1 );

		if( io_ctrl->verify_mode & 1 ){

			result = read_block( dev, offset, &res_block, block_size );
			if( !result && !io_ctrl->ignore_error )
				exit( -1 );

			if( !compare_blocks(block,res_block, block_size, offset ) ){
				dev->error_count++;
				if( !io_ctrl->ignore_error )
					exit(-1);
			}
			free( res_block );
			res_block = NULL;

		}

		offset 		+= block_size;
		block_size 	 = get_next_block_size( offset, dev->size, io_ctrl->block_size, io_ctrl->end );

	}

	/*verification mode 2 (read all after writing all) */

	if( io_ctrl->verify_mode & 2 ){

		if( io_ctrl->delay && dev->size ){
			msg_sleep(io_ctrl->delay);
		}

		offset	   = io_ctrl->start;
		block_size = get_next_block_size (offset, dev->size, io_ctrl->block_size, io_ctrl->end );

		while (block_size){

			result = read_block( dev, offset, &res_block, block_size );
			if( !result && !io_ctrl->ignore_error )
				exit ( -1 );

			if( !compare_blocks( block,res_block,block_size, offset ) ){
				dev->error_count++;
				if( !io_ctrl->ignore_error )
					exit(-1);
			}
			free( res_block );
			res_block   = NULL;

			offset 	   += block_size;
			block_size  = get_next_block_size( offset, dev->size, io_ctrl->block_size, io_ctrl->end );

		}

	}
	free( block );
	clearline();
	printf( "\rdevice filled (pass #%d)          \n", ++counter );
	dev->pass_count++;

}




void inc_fill( dev_ctrl_t *dev, int start_offset, io_ctrl_t *io_ctrl ){
	long long block_size;
	long long offset = io_ctrl->start;
	unsigned char* block;
	unsigned char* res_block=NULL;
	int result;
	static int counter=0;
	static int iterator = 0;

	block_size 	= get_next_block_size( offset, dev->size, io_ctrl->block_size, io_ctrl->end );
	dev->block_size = block_size;

	if( !block_size ||!dev->size )
		printf("Unable to detect drive size, test skipped\n");

	res_block=NULL;
	while( block_size ){
		if( iterator == 0 )
			iterator = start_offset;
		block = create_test_block ( offset, dev->sector_size, block_size, counter, io_ctrl->mode );
		assert(block);
		result = write_block(dev, offset, block , block_size );
		if( !result && !io_ctrl->ignore_error  )
			exit( -1 );

		if( io_ctrl->verify_mode & 1 ){
			result = read_block( dev, offset, &res_block, block_size );
			if( !result && !io_ctrl->ignore_error )
				exit(-1);
			if( !compare_blocks( block, res_block,block_size, offset ) ){
				dev->error_count++;
				if( !io_ctrl->ignore_error)
					exit(-1);
			}
			free( res_block );
			res_block = NULL;
		}
		free(block);
		block=NULL;
		offset += block_size;
		block_size = get_next_block_size( offset, dev->size, io_ctrl->block_size, io_ctrl->end );
	}

	if( io_ctrl->verify_mode & 2 ){

		if(io_ctrl->delay && dev->size && block_size ){
			msg_sleep(io_ctrl->delay);
		}

		offset=io_ctrl->start;
		block_size = get_next_block_size (offset, dev->size,io_ctrl->block_size, io_ctrl->end );
		while( block_size ){
		    block = create_test_block ( offset, dev->sector_size, block_size, counter, io_ctrl->mode );
		    assert(block);
			result = read_block( dev, offset, &res_block, block_size );
			if(!result && !io_ctrl->ignore_error )
				exit (-1);
			if(!compare_blocks( block,res_block,block_size, offset ) ){
				dev->error_count++;
				if( !io_ctrl->ignore_error )
					exit(-1);
			}
			free(res_block);
			res_block=NULL;
			free(block);
			block=NULL;
			offset += block_size;
			block_size = get_next_block_size( offset, dev->size,io_ctrl->block_size, io_ctrl->end );
		}
	}
	clearline();
	iterator=offset;
	if(dev->size){
		dev->pass_count++;
		printf("\rpass #%d ended                \n",++counter);
	}

}


void read_test( dev_ctrl_t* dev, io_ctrl_t* io_ctrl ){
	long long block_size;
	long long offset = io_ctrl->start;
	unsigned char* block=NULL;
	static int counter=0;
	int result;

	block_size 	= get_next_block_size (offset, dev->size,io_ctrl->block_size, io_ctrl->end );
	dev->block_size = block_size;

	while (block_size){
		result = read_block( dev, offset, &block, block_size );
		if( !result && !io_ctrl->ignore_error)
			exit(-1);
		offset+=block_size;
		block_size = get_next_block_size (offset, dev->size, io_ctrl->block_size, io_ctrl->end );
	}
	free( block );
	clearline();
	printf("\rread test (pass #%d)             \n",++counter);
	dev->pass_count++;

}

void save_image( dev_ctrl_t* dev, const char* name, io_ctrl_t* io_ctrl ){
	HANDLE sav_target;
	long long block_size=0;
	unsigned char* block=NULL;
	long long offset=io_ctrl->start;
	int result;
	int std_flag=0;
	int pref_size=0;
	DWORD written;
	if( !strncmp(name,"-",2)){

		sav_target=GetStdHandle(STD_OUTPUT_HANDLE);
		std_flag=1;
		if( io_ctrl->block_size )
			pref_size=io_ctrl->block_size;
		else
			pref_size=4096;

	}else{
		sav_target = CreateFile( name, GENERIC_WRITE|GENERIC_READ, 0, NULL, CREATE_NEW, 0, NULL );
	}
	if(sav_target==INVALID_HANDLE_VALUE){
		printf("Unable to open file %s, %s\n",name, error_text());
		return;
	}
	dev->block_size = pref_size;
	block_size = get_next_block_size (offset, dev->size, pref_size, io_ctrl->end );
	while( block_size ){
		result= read_block( dev, offset, &block, block_size );
		if( !result && !io_ctrl->ignore_error )
			exit(-1);
		result=WriteFile( sav_target, block, (DWORD)block_size,(LPDWORD)&written, NULL );		
		if( !result ){
			printf("error: %s\n",error_text());
			if( !io_ctrl->ignore_error )
				exit(-1);
		}
		if(written!=(DWORD)block_size){
			printf("block not fully copyied (%u of %I64d)\n",(unsigned int)written,block_size);
			if(! io_ctrl->ignore_error)
				exit(-1);
		}

		free(block);
		block=NULL;
		offset+=block_size;
		block_size = get_next_block_size (offset, dev->size, pref_size, io_ctrl->end );
	}
	if(!std_flag)
		CloseHandle(sav_target);
	free(block);
	printf("\nsave finished\n");
	dev->pass_count++;

}


void load_image( dev_ctrl_t* dev, const char* name, io_ctrl_t* io_ctrl ){

	HANDLE		load_target;
	long long	block_size	=	0;
	unsigned char*	block		=	NULL;
	unsigned char*	res_block	=	NULL;
	long long 	offset		=	io_ctrl->start;
	int 		result;
	int 		std_flag	=	0;
	int 		pref_size	=	0;
//	DWORD written;
	DWORD 		loaded;


	if( !strncmp(name,"-",2)){/*stdin*/

		load_target = GetStdHandle( STD_INPUT_HANDLE );
		if( load_target == INVALID_HANDLE_VALUE ){
			printf( "Unable to open stdin (bug?)\n");
			return;
		}
		std_flag = 1;
		if( io_ctrl->block_size )
			pref_size = io_ctrl->block_size;
		else
			pref_size = 4096;

	}else{
		load_target	= CreateFile( name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
		pref_size	= io_ctrl->block_size;
	}
	if( load_target == INVALID_HANDLE_VALUE ){
		printf(" Unable to open file %s. %s\n", name, error_text() );
		return;
	}

	dev->block_size = pref_size;
	block_size = get_next_block_size (offset, dev->size, pref_size, io_ctrl->end );

	while( block_size ){
		
		block = malloc( (size_t)block_size );
		assert(block);

		result = ReadFile( load_target, block, (DWORD)block_size, (LPDWORD)&loaded, NULL );
		if( !result ){/*io_ctrl->ignore_error do not work here!*/
			printf("file %s:%s\n", name, error_text() );
			exit(-1);
		}
		if( !loaded )
			break;
		if(block_size!=loaded){
			if(dev->sector_size){
				if( loaded%dev->sector_size ){
					printf("\nWarning: size of file %s do not match sector size for %s (%I64d b)\n",name, dev->user_name, dev->sector_size);
					memset(block+loaded,0,block_size-loaded);
					if((loaded/dev->sector_size+1)*dev->sector_size <= block_size )
						block_size = (loaded/dev->sector_size+1)*dev->sector_size;
					else{
						printf("\nInternal error, sector size is too large (%I64d)\n",dev->sector_size);
						exit(-1);
					}
				}else
					block_size = loaded;
			}else
				block_size=loaded;
		}
		result = write_block( dev, offset, block, block_size );
		if(!result && !io_ctrl->ignore_error )
				exit (-1);

		if( io_ctrl->verify_mode ){
			res_block = malloc( (size_t)block_size );
			result = read_block( dev, offset, &res_block, block_size );
			if( ! result && !io_ctrl->ignore_error )
				exit(-1);
			if(!compare_blocks( block,res_block,block_size, offset ) ){
				dev->error_count++;
				if(!io_ctrl->ignore_error)
					exit(-1);
			}
			free( res_block );
		}
		offset +=  block_size;
		block_size =  get_next_block_size( offset, dev->size, pref_size, io_ctrl->end );
		free (block);
	}
	if(!std_flag)
		CloseHandle( load_target );
	printf("\nimage load finished\n");
	dev->pass_count++;
}


void quick_test ( dev_ctrl_t* dev, io_ctrl_t* io_ctrl ){
/*content of test: try to write/read a base*sector_size block in each block_size block

*/
	long long	test_block_size		= 0;
	long long	skipped_block_size	= 0;	/*include test_block_size*/
	long long	current_offset 		= 0;	/*todo -r option support*/
	unsigned char*	primary			= NULL;
	unsigned char*	secondary		= NULL;
	unsigned char*  pattern			= NULL;


	if( dev->sector_size )
		test_block_size    = dev->sector_size;
	else
		test_block_size    = 512;

	if ( io_ctrl->block_size )
		skipped_block_size = io_ctrl->block_size;
	else
		skipped_block_size = 65536;

	if( test_block_size <= 0 || skipped_block_size <=0 || test_block_size < skipped_block_size ){
		test_block_size    = 512;
		skipped_block_size = 65536;
	}
	
	primary   = calloc( test_block_size, 1 );
	secondary = calloc( test_block_size, 1 );
	pattern	  = create_test_block( current_offset, test_block_size, test_block_size, 0, 1);

	assert( primary && secondary );

	while( current_offset < dev->size ){
		if( !read_block( dev, current_offset, &primary, test_block_size )  && !io_ctrl->ignore_error )
			return;
/*		if( !write_block( dev, current_offset, pattern, test_block_size ) && !io_ctrl->ignore_error )
			return;
		if( !read_block( dev, current_offset, &secondary, test_block_size ) && !io_ctrl->ignore_error )
			return;
		if( !compare_blocks( pattern, secondary, test_block_size, current_offset ) ){
			dev->error_count++;
			if( !io_ctrl->ignore_error )
				return;
		}*/
		if( !write_block( dev, current_offset, primary, test_block_size ) && !io_ctrl->ignore_error )
			return;
		if( !read_block( dev, current_offset, &secondary, test_block_size ) && !io_ctrl->ignore_error )
			return;
		if( !compare_blocks( primary, secondary, test_block_size, current_offset ) ){
			dev->error_count++;
			if( !io_ctrl->ignore_error )
				return;
		}
		current_offset+=skipped_block_size;
	}
	if( !dev->error_count )
		printf( "\nquick test passed, no error founds, please run a full test (-T)\n" );
	else
		printf( "\nquick test found a %I64d errors\n", dev->error_count );
	dev->pass_count++;
	free(primary);
	free(secondary);	

}


void update_action(  dev_ctrl_t* dev, io_ctrl_t* io_ctrl ){

	long long 	block_size;
	long long 	offset		= io_ctrl->start;
	unsigned char*	primary		= NULL;
	unsigned char*	secondary	= NULL;

	block_size 	= get_next_block_size (offset, dev->size,io_ctrl->block_size, io_ctrl->end );
	dev->block_size = block_size;

	primary = calloc( block_size, 1 );
	secondary = calloc( block_size, 1 );
	assert( primary && secondary );
	
	while (block_size){
	
		if( !read_block( dev, offset, &primary, block_size )  && !io_ctrl->ignore_error )
			return;
		if( !write_block( dev, offset, primary, block_size ) && !io_ctrl->ignore_error )
			return;
		if( !read_block( dev, offset, &secondary, block_size ) && !io_ctrl->ignore_error )
			return;
		if( !compare_blocks( primary, secondary, block_size, offset ) ){
			dev->error_count++;
			if( !io_ctrl->ignore_error )
				return;
		}
		offset += block_size;
		block_size = get_next_block_size( offset, dev->size, io_ctrl->block_size, io_ctrl->end );
	}
	free( secondary );
	free( primary );

}





void get_drive_info(dev_ctrl_t* dev, int scan, int silence ){



	if(scan & FLASHNUL_GEOMETRY_SCAN){

		if( get_drive_geometry( dev ) ){
			dev->drive_geometry_flag = 1;
		}
		else{
			if( !silence )
				printf("DeviceIoCtrl() / IOCTL_DISK_GET_DRIVE_GEOMETRY failed: %s",error_text());
			dev->detection_error++;
		}

	}

	if( !dev->physical_flag && scan & FLASHNUL_FREESPACE_SCAN ){

		if( get_free_space( dev ) )
			dev->disk_space_flag = 1;
		else
			if( !silence )
				printf("GetFreeDiskSpaceEx() failed: %s", error_text() );
		
	}

	if( scan &  FLASHNUL_DEVICE_SIZE_SCAN ){

		dev->device_size = get_device_size( dev );
		if( dev->device_size >= 0 )
			dev->drive_length_flag = 1;
	    	else{
	    	        dev->detection_error++;
			if( !silence )
				printf( "DeviceIOControl() / IOCTL_DISK_GET_LENGTH_INFO failed: (%I64d) %s", dev->device_size, error_text() );
			if( dev->device_size == -1 )
				dev->device_size =0;
		}

	}

	if( scan & FLASHNUL_ADAPTER_PROPERTIES_SCAN ){

		if( get_adapter_property( dev ) )
			dev->adapter_flag = 1;
		else
			if( !silence )
				printf( "DeviceIOControl() / IOCTL_STORAGE_QUERY_PROPERTY (adapter) failed: %s", error_text() );
		

	}

	if( scan & FLASHNUL_DEVICE_PROPERTIES_SCAN ){

		if( get_device_property( dev ) )
			dev->device_flag = 1;
		else
			if( !silence )
				printf( "DeviceIOControl() / IOCTL_STORAGE_QUERY_PROPERTY (device) failed: %s", error_text() );		

	}

	if( scan & FLASHNUL_HOTPLUG_SCAN ){
		if(! get_device_hotplug_info( dev ) )
			if(! silence )
				printf( "DeviceIOControl() / IOCTL_STORAGE_GET_HOTPLUG_INFO filed: %s", error_text() );
	}
	select_device_size( dev );
}

void select_device_size ( dev_ctrl_t* dev ){
        if(isdigit(dev->user_name[0])||dev->user_name[0]=='\\'){ /*physical drive or UNC*/
		dev->size = max( max(dev->geometry_size, dev->disk_size), dev->device_size );
        }
        if(isalpha(dev->user_name[0])){ /*logical drive*/
        	if( dev->device_size ) 
        		dev->size = dev->device_size ;
		else
			dev->size = max( max(dev->geometry_size, dev->disk_size), dev->device_size );
       }
}


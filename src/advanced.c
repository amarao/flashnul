/* 

	Advanced recovery mode allow to save image (not fully) from
	hanguping devices. Standard recovery software do not allow to
	unplug devices during saving and plug it back without restarting
	recovery process.

	This mode deisgned espesially for flash (sd/mmc/etc) and (may be)
	for CD (dvd/bd) disks.

	Algorithm:

		saving image. If error happens:
			* close device
			* wait for user conformation (and repluging a hangupped device)
			* reopen device
			* seek to error place + offset
			* fill a skipped space by fill_byte value in output file
			* continue saving
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/*void advanced_recovery (
			const char* dev_name, 
			const char* name, 
			const char* skip, //converted to number
			const char* fill, //converted to number
			){

	HANDLE		input_device;
	HANDLE		output_file;
	FILE*		log_file;
	unsigned char*	output_name[MAX_PATH+1];
	unsigned char 	log_name[MAX_PATH+1];
	unsigned char   fill_byte;
	long long	skip_size;
	long long	offset = 0;
	
	snprintf( output_name, MAX_PATH, "%s.ARimage",name );
	snprintf( log_name, MAX_PATH, "%s.ARlog",name );

	output_file = CreateFile( output_name, GENERIC_WRITE|GENERIC_READ, 0, NULL, CREATE_NEW, 0, NULL );
	if( output_file == INVALID_HANDLE_VALUE ){
		printf( "sorry, unable to create new file %s\n", output_file );
		return;
	}
	output_log = fopen(log_name, "w");
	if( !output_log ){
		printf( "sorry, unable to open log file %s\n", output_log );
		return;
	}

	printf("** Advanced recovery mode started. If device hand up during saving, unplug it, plug back and press [Enter] key (or input a different seek value)\n");



}
*/

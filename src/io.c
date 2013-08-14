#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <winioctl.h>
#include <winbase.h>
#include <string.h>
#include <ctype.h>
#include "finetune.h"
#include "ddk.h"
#include "io.h"
#include "common.h"
#include "display.h"

char* error_text(){ 

    static char  message[MAX_PATH*2];
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

	CharToOem(lpMsgBuf,lpMsgBuf);    	
	strncpy(message,lpMsgBuf,MAX_PATH*2-1);	
    LocalFree(lpMsgBuf);
    return message;
}


int	get_free_space(dev_ctrl_t* dev){

	char 		*dir		=	malloc( strlen( dev->system_name ) + 2 );
	ULARGE_INTEGER 	free_avaible;
	ULARGE_INTEGER 	total;
	ULARGE_INTEGER 	free;
	int		status;
	
	assert( dir );
	strcpy( dir, dev->system_name );
	strcat( dir, "\\" );

	status = GetDiskFreeSpaceEx( dir,(PULARGE_INTEGER)&free_avaible, (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free );

	if( status ){	
		dev->disk_size		= (long long) total.QuadPart;
		dev->free_size		= (long long) free.QuadPart;
		dev->avaible_size	= (long long) free_avaible.QuadPart;
	}
	return status;

}

int get_drive_geometry(dev_ctrl_t* dev){

	int result;
	DWORD junk;
	DISK_GEOMETRY disk_geometry;

	result = DeviceIoControl ( 
		dev->device_handle, 
		IOCTL_DISK_GET_DRIVE_GEOMETRY, 
		NULL, 0, 
		&disk_geometry, 
		sizeof( DISK_GEOMETRY ), 
		&junk, 
		(LPOVERLAPPED) NULL 
	);

	if( result ){

		dev->sector_size 	= (long long)disk_geometry.BytesPerSector;
		dev->cylinders		= (long long)disk_geometry.Cylinders.QuadPart;
		dev->tracks_per_cylinder= (long long)disk_geometry.TracksPerCylinder;
		dev->sectors_per_track	= (long long)disk_geometry.SectorsPerTrack;
		dev->geometry_size	= dev->sector_size * dev->cylinders * dev->tracks_per_cylinder * dev->sectors_per_track;
	}
	return result;

}

char* get_full_name(char* alias){
	static char buf[MAX_PATH];
	int offset = 0;
	if(alias[0]=='\\') 
		offset = 1;
	snprintf(buf,MAX_PATH,"PhysicalDrive%s",alias+offset);
	return buf;
}


void get_perfomance_frequency( dev_ctrl_t* dev ){
/*get a frequency of the high-resolution performance counter*/

	LARGE_INTEGER freq;

	if( QueryPerformanceFrequency( &freq ) ){
		dev->freq 		= freq.QuadPart;
		dev->update_counts	= ( REFRESH_DELAY * dev->freq ) / 1000;
	}else{
		printf( "Unable to obtain a sysem speed, statistic disabled. %s\n", error_text() );
		dev->freq = 0;
	}
}


dev_ctrl_t* open_drive( char* device_path, int silence, int write_share){

	const char prefix[] = "\\\\.\\";
	char* uname;
	DWORD junk;
	dev_ctrl_t* retval = calloc( sizeof(dev_ctrl_t), 1 );
	
	assert( retval );

	if( device_path[0]=='\\' && device_path[1] == '\\' ){
		retval->user_name = strdup( device_path );
		retval->system_name = strdup( device_path );
	}
	else{
		if( isdigit(device_path[0]) || ( device_path[0]=='\\' && isdigit(device_path[1]) ) ){ /*aliases for \\.\PhysicalDriveX*/
			uname = get_full_name( device_path );
			retval->physical_flag=1;
		}else{
			uname=device_path;
			retval->physical_flag=0;
		}
		retval->user_name=strdup( uname );
		retval->system_name = malloc( strlen( uname ) + strlen(prefix) + 1 );
		assert( retval->user_name && retval->system_name );
		strcpy(retval->system_name, prefix);
		strcat(retval->system_name, uname );
	}
	retval->device_handle = CreateFile( 
		retval->system_name, 
		GENERIC_WRITE|GENERIC_READ, 
		FILE_SHARE_READ |(write_share?FILE_SHARE_WRITE:0),
		NULL, 
		OPEN_EXISTING, 
		0, 
		NULL
	);

	if( retval->device_handle==INVALID_HANDLE_VALUE ){
		if(!silence)
			printf("error opening drive %s (%s): %s\n",retval->user_name, retval->system_name,error_text() );
		free( retval->user_name );
		free( retval->system_name );
		free( retval );
		return NULL;
	}

	DeviceIoControl( retval->device_handle,FSCTL_ALLOW_EXTENDED_DASD_IO, NULL,0,NULL,0,&junk,NULL);

	retval->write_share = write_share;

	get_perfomance_frequency( retval );

	return retval;	

}

void close_drive(dev_ctrl_t* dev){

	CloseHandle ( dev->device_handle );
	free( dev->user_name );
	free( dev->system_name );
	free( dev->bus_name );
	free( dev->name );
	free( dev->vendor );	
	free( dev->revision );	
	free( dev->raw_serial );
	free( dev->serial );
	free( dev );

}

static inline long long get_timer(){
	LARGE_INTEGER counter;
	if(QueryPerformanceCounter(&counter))
		return counter.QuadPart;
	else
		return 0;
}

int write_block(dev_ctrl_t* dev, long long offset,unsigned char* data, long long data_size ){
	int 		res;
	DWORD 		write;
	LARGE_INTEGER	move;
	long long 	begin;
	long long 	end;
	long long 	count;
	long long 	speed; /*(b/s)*/

	move.QuadPart = offset;

	dev->counter2 = get_timer();

	if(dev->counter2 - dev->counter1 > dev->update_counts || !dev->counter2 ){
	        clearline();
		fprintf(stderr,"\rWriting 0x%I64x (%s)",offset, human_view( offset+data_size, 'b' ) );
		if(dev->freq&&dev->write_counts)
			fprintf(stderr,", %I64d b/s", MULDIV(dev->write_bytes,dev->freq,dev->write_counts));
		dev->counter1=dev->counter2;
	}

	SetFilePointerEx(dev->device_handle, move, NULL,FILE_BEGIN);
	begin = get_timer();
	res=WriteFile( dev->device_handle, data, data_size,(LPDWORD)&write, NULL );
	end = get_timer();
	if( !res ){
		dev->error_count++;
		fprintf(stderr,"\nWrite error: %s\n",error_text());
		return 0;
	}
	dev->write_bytes+= write;
	if( dev->freq ){
		count = end - begin;
		dev->write_counts += count;
		if(dev->block_size==data_size && count > 0 ){ /*skip leftover blocks*/
			speed = MULDIV (write, dev->freq, count);
			if( speed < dev->min_write_speed || !dev->min_write_speed )
				dev->min_write_speed = speed;
			if( speed > dev->max_write_speed )
				dev->max_write_speed = speed;
		}
	}



	if( write!=data_size ){
		dev->error_count++;
		fprintf( stderr,"\nerror, only %u bytes written (insead %I64d)\n",(unsigned int)write, data_size );
		return 0;
	}
	return 1;
}

int read_block(dev_ctrl_t* dev, long long offset, unsigned char** data, long long data_size ){

	int 		res;
	DWORD 		read;
	LARGE_INTEGER	move;
	long long 	begin;
	long long 	end;
	long long 	count;
	long long 	speed; /*(b/s)*/

	move.QuadPart = offset;

	dev->counter2 = get_timer();

	if(dev->counter2 - dev->counter1 > dev->update_counts || !dev->counter2 ){
	        clearline();
		fprintf(stderr, "\rReading 0x%I64x (%s)", offset, human_view(offset, 'b') );
		if(dev->freq&&dev->read_counts)
			fprintf(stderr,", %I64d b/s", MULDIV (dev->read_bytes,dev->freq,dev->read_counts));
		dev->counter1=dev->counter2;
	}

	SetFilePointerEx( dev->device_handle, move, NULL, FILE_BEGIN );
	if(!*data){
		*data=malloc( data_size );
		assert( *data );
	};
	begin = get_timer();
	res   = ReadFile( dev->device_handle, *data, data_size, (LPDWORD)&read, NULL );
	end   = get_timer();
	if( !res ){
		dev->error_count++;
		fprintf(stderr,"\nRead error: %s\n",error_text());
		return 0;
	}

	dev->read_bytes += read;
	if( dev->freq ){
		count = end - begin;
		dev->read_counts += count;
		if(dev->block_size==data_size && count > 0 ){ /*skip leftover blocks*/
			speed = MULDIV( read, dev->freq, count );
			if( speed < dev->min_read_speed || !dev->min_read_speed )
				dev->min_read_speed = speed;
			if( speed > dev->max_read_speed )
				dev->max_read_speed = speed;
		}
	}

	if( read != data_size ){
		dev->error_count++;
		fprintf (stderr,"\nerror, only %u bytes read (insead %I64d)\n",(unsigned int)read, data_size );
		return 0;
	}

	return 1;

}


long long get_device_size(dev_ctrl_t* dev ){
/*return a real device size*/
	int			result;
	DWORD			junk;
	long long		retval;
	GET_LENGTH_INFORMATION	length_info;
	result=DeviceIoControl(
		dev->device_handle,
		IOCTL_DISK_GET_LENGTH_INFO,
		NULL,0,
		&length_info,
		sizeof( length_info ),
		&junk,
		(LPOVERLAPPED) NULL
	);
	if(result)
		retval = (long long)(length_info.Length.QuadPart);
	else
		retval = -1;
	return retval;


}
	

char* get_bus_name (const int bus_type){
	static char* bus_names[] = {
    "UNKNOWN",  // 0x00
    "SCSI",
    "ATAPI",
    "ATA",
    "IEEE1394",
    "SSA",
    "FIBRE",
    "USB",
    "RAID",
    "ISCSI",
    "SAS",
    "SATA"
};
    if(bus_type<0||bus_type>=sizeof(bus_names)/sizeof(*bus_names))
		return strdup(bus_names[0]);
	return strdup(bus_names[bus_type]);

}

char* get_name (const char* buffer, int text_offset,int max_len){
    char* ret;
	if(!text_offset)
		return "";
	ret=malloc(max_len-text_offset+1);
	strncpy(ret,buffer+text_offset,max_len-text_offset);
	ret[max_len-text_offset]=0;

	return ret;
}


void remove_trail_spaces(char* line){
	int c;
	for(c=strlen(line)-1;c;c--){
		if(line[c]==' '||line[c]=='\t')
			line[c]=0;
		else
			break;
	}
}

char* clean_serial( const char* serial ){
#define HEX(a) (isalpha(a)?(isupper(a)?(a-'A'+10):(a-'a'+10)):(a-'0'))
	int len = strlen(serial);
	int c;
	char* newline;
	int endline=0;

	for( c = 0; c < len; c++ ){
		endline=c;
		if( !isxdigit(serial[c]) && !isdigit(serial[c]) )
			break;

	}
	endline = (endline+1)&0xFFFFFFFC; /*making multipy to 4*/

	if( !endline ) 
		return strdup( serial );

	
	newline = malloc( len - endline/2 + 3 );/* len(newline)=len(decoded serial)+len(leftover)+3*/
	assert( newline );

	for( c = 0; c < endline; c += 4 ){
		newline[c/2]   = HEX(serial[c+2])*16+HEX(serial[c+3]);
		newline[c/2+1] = HEX(serial[c])*16+HEX(serial[c+1]);
	}

	newline[c/2] = 0;

	remove_trail_spaces(newline);	
	
	if(endline!=len){
		strcat( newline, " /" );
		strcat( newline, serial + endline );
	}

	return newline;
}


int get_adapter_property( dev_ctrl_t* dev ){

	#define BUFFER_SIZE 2047

	STORAGE_PROPERTY_QUERY		query;
	BOOL				status;
	UCHAR				buffer[BUFFER_SIZE+1];
	ULONG				returned_length;

	query.PropertyId = StorageAdapterProperty;
	query.QueryType  = PropertyStandardQuery;

	status = DeviceIoControl(dev->device_handle,
		        	IOCTL_STORAGE_QUERY_PROPERTY,
		                &query,
		                sizeof( STORAGE_PROPERTY_QUERY ),
		                &buffer,
		                BUFFER_SIZE,                      
		                &returned_length,      
		                NULL                    
               		);

	if (status ) {
		dev->bus_type = ( (PSTORAGE_ADAPTER_DESCRIPTOR)buffer )->BusType;
		dev->bus_name = get_bus_name( dev->bus_type );
	}else
		dev->bus_name = strdup( "(error while detecting)" );

       	return status;

}


int get_device_property( dev_ctrl_t* dev ){

	STORAGE_PROPERTY_QUERY		query;
	PSTORAGE_DEVICE_DESCRIPTOR	device;
	BOOL				status;
	UCHAR				buffer[BUFFER_SIZE+1];
	ULONG				returned_length;


	query.PropertyId = StorageDeviceProperty;
	query.QueryType  = PropertyStandardQuery;

	status = DeviceIoControl(
    		dev->device_handle,
			IOCTL_STORAGE_QUERY_PROPERTY,
			&query,
			sizeof( STORAGE_PROPERTY_QUERY ),
			&buffer,
			BUFFER_SIZE,
			&returned_length,
			NULL
		);

        if ( status ) {

		device = (PSTORAGE_DEVICE_DESCRIPTOR) buffer;

		dev->removable	= device->RemovableMedia;
		dev->CQ 	= device->CommandQueueing;
		dev->vendor	= get_name(buffer,device->VendorIdOffset,returned_length);
		dev->name	= get_name(buffer,device->ProductIdOffset,returned_length);
		dev->revision	= get_name(buffer,device->ProductRevisionOffset,returned_length);
		dev->raw_serial	= get_name(buffer,device->SerialNumberOffset,returned_length);
		dev->serial	= clean_serial(dev->raw_serial);
	}else{
            	dev->vendor	= strdup( "" );
            	dev->name	= strdup( "" );
            	dev->revision	= strdup( "" );
            	dev->raw_serial	= strdup( "" );
	    	dev->serial	= strdup( "" );

        }

	return status;

}


int get_device_hotplug_info (dev_ctrl_t* dev ){
	OSVERSIONINFOEX version;
	STORAGE_HOTPLUG_INFO stor;
	BOOL status;
	DWORD trash;

	version.dwOSVersionInfoSize=sizeof(version);

	status = GetVersionEx( (LPOSVERSIONINFO )&version );
	if(!status)
		return 0; /*error*/

	if(version.dwMajorVersion < 5 ) /*winNT*/
		return 1; /*not a error, but no info avaible*/

	if(version.dwMajorVersion == 5 && version.dwMinorVersion == 0 ) /*win2k*/
		return 1; /*not a error, but no info avaible*/

	/*ok, wXP, vista or higher*/

	status = DeviceIoControl(
			dev->device_handle,
			IOCTL_STORAGE_GET_HOTPLUG_INFO,  
			NULL,
			0,
			&stor,
			sizeof(stor),
			&trash,
			NULL
		);

	if(!status)
		return 0; /*error*/

	dev->hotplug_info_flag	= 1;
	dev->media_hotplug	= stor.MediaHotplug;
	dev->device_hotplug	= stor.DeviceHotplug;

	return 1;

}

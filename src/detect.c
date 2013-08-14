#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "detect.h"
#include "common.h"

DWORD get_drive_type(char* path){
	char buffer[MAX_PATH];
	strncpy(buffer,path,MAX_PATH);
	strncat(buffer,"\\",MAX_PATH);
	return GetDriveType(buffer);
}



const char* get_drive_type_string(char* path){

	int c;
	struct drive_string_list_s{
		DWORD ID;
		const char* string;
	}strings[]={
		{ DRIVE_UNKNOWN, 	"Unknown"				},
		{ DRIVE_NO_ROOT_DIR,	"No root dir (not mounted device)"	},
		{ DRIVE_REMOVABLE, 	"Removable"				},
		{ DRIVE_FIXED, 		"Fixed"					},
		{ DRIVE_REMOTE, 	"Network or remote device"		},
		{ DRIVE_CDROM,		"CD/DVD"				},
		{ DRIVE_RAMDISK,	"RAM-drive"				}
	};
	DWORD type=get_drive_type( path );
	for(c=0;c<sizeof(strings)/sizeof(strings[0]);c++){
		if(type==strings[c].ID)
			return strings[c].string;
	}
	return strings[0].string;
}



void list_logical_drives(int flags){

	DWORD 	req_space;
	DWORD 	res_space;
	char* 	buff;
	char* 	curr;
	int 	curr_len;

	req_space = GetLogicalDriveStrings( 0, NULL );
	if(!req_space || req_space>65535 ){
		printf("GetLogicalDriveStrings() fails\n");
		return;
	}


	buff=malloc(req_space+1);
	assert(buff);
	curr=buff;
	res_space = GetLogicalDriveStrings( req_space, buff );
	printf("\nAvaible logical disks:\n");
	while( *curr ){
		curr_len = strlen( curr );
		printf( "%s\t", curr );
		if( !flags & 0x0100 )
			printf("type=%u (%s)",
				(unsigned int)get_drive_type(curr),
				get_drive_type_string(curr)
			);
		printf("\n");
		curr+=curr_len+1;
	}
}


void list_physical_drives(){
	char d1;
	char d2;
	dev_ctrl_t* dev;
	char pattern[]="PhysicalDrive$$";
	assert(pattern[13]=='$'&&pattern[14]=='$');
	printf("\nAvaible physical drives:\n");

	for(d1='0';d1<='9';d1++){
		for(d2='0';d2<='9';d2++){
		        if(d1=='0'){
				pattern[13]=d2;
				pattern[14]=0;
			}else{
				pattern[13]=d1;
				pattern[14]=d2;
			}
			dev = open_drive( pattern, 1, 0 );
			if(dev){
				printf("%d\tsize = %I64d (%s)\n",
					(d1-'0')*10+d2-'0', 
					get_device_size(dev), 
					human_view(get_device_size(dev), 'b')
				);
				close_drive(dev);
			}
		}

	}
	
}



void list_disks(int flags){

	int display = flags?flags:0xFF;

	if(display&0x1) 
		list_physical_drives();
	if(display&0x2)
		list_logical_drives(display&0xFF00);
}

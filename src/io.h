#ifndef _H_IO_H
#define _H_IO_H
#include <windows.h>

typedef struct{

	HANDLE		device_handle;	/*handle to open drive */
	int		write_share;	/*write share flag (1 - write share enabled)*/
	int		physical_flag;	/*opened device is a physical device =1, 0 - logical */
	char*		user_name;	/*name of device from command line*/
	char*		system_name;	/*name of device, as passed to CreateFile() */
	long long	size; 		/*device size (collected from diff.sources) */
	long long	block_size;	/*size of the r/w block*/

	/*reported from IOCTL_DISK_GET_DRIVE_GEOMETRY */
	int		drive_geometry_flag;
	long long	sector_size;
	long long	cylinders;
	long long	tracks_per_cylinder;
	long long 	sectors_per_track;
	long long	geometry_size;		/*note: not a real size*/

	/*reported from GetDiskFreeSpaceEx()*/
	int		disk_space_flag;
	long long	disk_size; 		
	long long	free_size;
	long long	avaible_size;

	/*reported from IOCTL_DISK_GET_LENGTH_INFO*/
	int		drive_length_flag;
	long long 	device_size;		/*most trusted value*/
	
	/*reported from IOCTL_STORAGE_QUERY_PROPERTY (adapter)*/
	int		adapter_flag;	
	int		bus_type;
	char*		bus_name;

	/*reported from IOCTL_STORAGE_QUERY_PROPERTY (device)*/	
	int		device_flag;
	int		removable;
	int 		CQ;			/*NCQ, TCQ, ect*/
	char* 		vendor;
	char* 		name;
	char* 		revision;
	char* 		raw_serial;
	char* 		serial;

	/*reported from IOCTL_STORAGE_GET_HOTPLUG_INFO (XP, Vista or higher)*/
	int		hotplug_info_flag;
	int		media_hotplug;
	int		device_hotplug;

	/*statistic*/
	long long	freq;			/*timer freq*/
	long long	update_counts;		/*number of counts for screen update*/
	long long	counter1;
	long long 	counter2;
	long long	read_bytes;		
	long long	write_bytes;
	long long	read_counts;		/*counts of timer for read operations*/
	long long	write_counts;		/*counts of timer for write operations*/
	long long	min_read_speed;
	long long	min_write_speed;
	long long	max_read_speed;
	long long	max_write_speed;
	

	long long 	error_count;
	long long 	pass_count;	
	int		detection_error;	/*error in detection*/


}dev_ctrl_t;



char*	error_text();

dev_ctrl_t*	open_drive( char* device_path, int silence, int write_share );
void		close_drive( dev_ctrl_t* dev );

int	get_free_space( dev_ctrl_t* dev );
int	get_drive_geometry( dev_ctrl_t* dev );

int	get_device_property( dev_ctrl_t* dev);
int	get_adapter_property( dev_ctrl_t* dev );

long long get_device_size( dev_ctrl_t* dev );
int get_device_hotplug_info (dev_ctrl_t* dev );

int read_block(dev_ctrl_t* dev, long long offset, unsigned char** data, long long data_size );
int write_block(dev_ctrl_t* dev, long long offset,unsigned char* data, long long data_size );
void list_disks();

#endif

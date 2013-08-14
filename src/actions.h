#ifndef _H_ACTIONS_H
#define _H_ACTIONS_H
void blank( dev_ctrl_t* dev, io_ctrl_t* io_ctrl );
void fill( dev_ctrl_t* dev, int byte, io_ctrl_t* io_ctrl );
void inc_fill( dev_ctrl_t *dev, int start_offset, io_ctrl_t *io_ctrl );
void read_test( dev_ctrl_t* dev, io_ctrl_t* io_ctrl );
void save_image( dev_ctrl_t* dev, const char* name, io_ctrl_t* io_ctrl );
void load_image( dev_ctrl_t* dev, const char* name, io_ctrl_t* io_ctrl );
void quick_test ( dev_ctrl_t* dev, io_ctrl_t* io_ctrl );
void update_action(  dev_ctrl_t* dev, io_ctrl_t* io_ctrl );

void select_device_size ( dev_ctrl_t* dev );
void get_drive_info(dev_ctrl_t* dev, int scan, int silence );
enum{
	FLASHNUL_GEOMETRY_SCAN			= 0x1,
	FLASHNUL_FREESPACE_SCAN			= 0x2,
	FLASHNUL_DEVICE_SIZE_SCAN		= 0x4,
	FLASHNUL_ADAPTER_PROPERTIES_SCAN	= 0x8,
	FLASHNUL_DEVICE_PROPERTIES_SCAN 	= 0x10,
	FLASHNUL_HOTPLUG_SCAN			= 0x20,
	FLASHNUL_DEBUG_INFO			= 0x80
};



#endif

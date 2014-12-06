#include <stdarg.h>
#include "io.h"
#include "generate.h"
#include "display.h"
#include "colian/colian.h"
#include "flashnul.h"
#include "actions.h"
#include "detect.h"
#include "finetune.h"
#include "advanced.h"

static const char* app_description =	"\nFlashnul - flash drive tester and cleaner\nUsage: flashnul <driveletter:> [OPTIONS]\nWarning: mostly all operations very DESTRUCTIVE and can cause a permanent information lose.\n Use carefully. Keep out of childen.\nPlease, avoid using near source of open flame.\nContra-indicated for people with reduced mental faculties.\n";
static const char* app_ver	     =	"flashnul 1.0rc1 (ex 0.994,  "__DATE__", "__TIME__")\n\n";
static const char* copyright	     =	"\n(c) George Shuklin, 2005-2008\nLicence: GNU Public Licence\nhomepage: www.shounen.ru/soft/flashnul/\nbugs mail to: gs@shounen.ru\n";
static const char* dump_warning    =  "\n\nIf you do not know how to use this program, please, read readme.rus.html\n";

enum{
	OPT_help,
	OPT_ver,
	OPT_fill,
	OPT_inc,
	OPT_read,
	OPT_blank,
	OPT_test,
	OPT_quick,
	OPT_update,
//	OPT_advanced,
	OPT_verify,
	OPT_do_not_confirm,
	OPT_ignore,
	OPT_cycle,
	OPT_probe,
	OPT_load,
	OPT_save,
	OPT_block_size,
	OPT_range,
	OPT_mode,
	OPT_delay,
	OPT_key,
	OPT_share,
//	OPT_alert,
	OPT_block_vendor,
	OPT_block_name,
	OPT_block_serial,
	OPT_block_bus,
	OPT_block_nonremovable,
	OPT_block_device,
	OPT_block_letter,
	OPT_block_number,
	OPT_ignore_block,
	OPT_title
};

claOptName_t opt[]={
{	OPT_help,		"help",		'h',	CLA_HELP,				0,	0,	"display help"			},
{	OPT_ver,		"version",	'v',	CLA_HELP,				0,	0,	"display version"		},
{	OPT_fill,		"fill",		'F',	CLA_TYPE_NUMBER|CLA_CMD|CLA_GROUP_0,	0,	1,	"fill drive by zeroes (wipe)" },
{	OPT_inc,		"incremental",'I',	CLA_TYPE_NUMBER|CLA_CMD|CLA_GROUP_0,	0, 	1,	"fill by incremented bytes (wipe)"},
{	OPT_read,		"read",		'R',	CLA_TYPE_NUMBER|CLA_CMD|CLA_GROUP_0,	0,	0,	"test drive for read (non destructive)"},
{	OPT_blank,		"blank",	'B',	CLA_TYPE_SWITCH|CLA_CMD|CLA_GROUP_0,	0,	0,	"erase a first sector (wipe)"},
{	OPT_test,		"test",		'T',	CLA_TYPE_SWITCH|CLA_CMD|CLA_GROUP_0,	0,	0,	"run standart test (-I -V=3 -m=1)"},
{	OPT_quick,		"quick",	'Q',	CLA_TYPE_SWITCH|CLA_CMD|CLA_GROUP_0,	1,	0,	"run a quick test"},
{	OPT_update,		"update",	'U',	CLA_TYPE_SWITCH|CLA_CMD|CLA_GROUP_0,	1,	0,	"update mode (write back)"},
//{	OPT_advanced,		"advanced-recovery", 0, CLA_TYPE_SWITCH|CLA_CMD|CLA_GROUP_0,	1,	3,	"save image from damaged media"},
{	OPT_verify,		"verify",	'V',	CLA_NUMBER,				1,	1,	"use a verification (mode = 1,2,3)"},
{	OPT_do_not_confirm,"no-prompt",'P', 		CLA_SWITCH,				0,	0,	"do not prompt about destructive functions"},
{	OPT_ignore,		"ignore-errors",'i',	CLA_SWITCH,				0,	0,	"ignore errors (do not stop after first error)"},
{	OPT_cycle,		"cycle",	'c',	CLA_NUMBER,				1,	1,	"run operation given number of loops (-R,I,F)"},
{	OPT_probe,		"probe",	'p',	CLA_TYPE_NUMBER|CLA_CMD|CLA_GROUP_0,	0,	1,	"list of avaible drives"},
{	OPT_load,		"load",		'L',	CLA_TYPE_STRING|CLA_CMD|CLA_GROUP_0,	1,	1,	"load file to drive (copy image from file to drive)"},
{	OPT_save,		"save",		'S',	CLA_TYPE_STRING|CLA_CMD|CLA_GROUP_0,	1,	2,	"save drive to file (copy image from drive to file)"},
{	OPT_block_size,		"block-size",'b',	CLA_NUMBER,				1,	1,	"set block size for read/writing (min sector size)"},
{	OPT_range,		"range",	'r',	CLA_NUMBER,				1,	2,	"set range of read/write operation (-R,S,L,I,F)"},
{	OPT_mode,		"mode",		'm',	CLA_NUMBER,				1,	1,	"change test mode for -I option"},
{	OPT_delay,		"delay",	'd',	CLA_NUMBER,				1,	1,	"delay before re-read data (-V=2,3)"},
{	OPT_key,		"disable-key",	'k',	CLA_SWITCH,				0,	0,	"do not prompt to press [Enter] at exit"},
{	OPT_share,		"write-share",	0,	CLA_SWITCH,				1,	1,	"enable share writing"},
//{	OPT_alert,		"alert",	'a',	CLA_SWITCH,				0,	0,	"sound altert on errors (beep)"},
{	OPT_block_vendor,	"block-vendor", 0,	CLA_TYPE_STRING|CLA_CFG,		1,	0,	"block write operations by vendor name"},
{	OPT_block_name,		"block-name",	0,	CLA_TYPE_STRING|CLA_CFG,		1,	0,	"block write operations by device name"},
{	OPT_block_serial,	"block-serial",	0,	CLA_TYPE_STRING|CLA_CFG,		1,	0,	"block write operations by device serial"},
{	OPT_block_bus,		"block-bus",	0,	CLA_TYPE_STRING|CLA_CFG,		1,	0,	"block write operations by adapter bus"},
{	OPT_block_nonremovable,	"block-non-removable",0,CLA_TYPE_SWITCH|CLA_CFG,		0,	0,	"block all non-removable devices"},
{	OPT_block_device,	"block-device",	0,	CLA_TYPE_STRING|CLA_CFG,		1,	0,	"block write by string in any field"},
{	OPT_block_letter,	"block-letter",	0,	CLA_TYPE_STRING|CLA_CFG,		1,	0,	"block write to logical drive by drive letter"},
{	OPT_block_number,	"block-number", 0,	CLA_TYPE_NUMBER|CLA_CFG,		1,	0,	"block write to physical drive by drive number"},
{	OPT_ignore_block,	"ignore-block", 0,	CLA_TYPE_SWITCH|CLA_CMD,		1,	0,	"ignore block options"},
{	OPT_title,		"title",	't',	CLA_SWITCH,				0,	0,	"use window title to show process"}
};


static char* old_title;
static DWORD old_title_size=0;

void title_init(){
	if(old_title_size) return;
	old_title=malloc(65535);
	if(!old_title){
		old_title_size=0; /* size=0 - not to free at exit and no change back*/
	}else{
		old_title_size=65535;
		GetConsoleTitle(old_title,old_title_size);
		old_title[65534]=0;/*paranoic*/
	}
	SetConsoleTitle(app_ver);
}


void title_text2 ( const char* text1, const char* text2 ){
	int text_size=strlen(text1)+strlen(text2)+strlen(app_ver)+4;
	char* text;
	if( text_size > 65535 ) return; /*spec of SetConsoleTitle*/
	text=malloc(text_size);
	if( !text && !old_title_size ) return;
	snprintf(text,text_size,"%s%s - %s",text1,text2,app_ver);
	SetConsoleTitle(text);
}

void title_dev_text( const char* op, dev_ctrl_t* dev ){
	return;
}

void title_restore(){
	if (old_title_size){
		SetConsoleTitle(old_title);
		free(old_title);
		old_title=NULL;
		old_title_size=0;
	}
}



int check_substring( claCfg_t* config, int optID, char* string ){
	int c;

	if( isactive(config,optID ) ){
		for(c = 0; c< argnum( config, optID ); c++ ){
			if(strstr( string, soptM( config, optID, c ) ) )
				return 1;
		}
	}
	return 0;
}

static char drive_letter(char* name){
	if(strlen(name)!=2)
		return 0;
	if(name[1]==':' )
		return name[0];
	return 0;
}

static int drive_number(char*name){
	int l=strlen(name);
	int end;
	int c;
	assert(l);
	for(c=l-1;c;c--)
		if(!isdigit(name[c])){
			end=c+1;
			break;
		}
	if( !c || c == l - 1 )
		return -1;
	return strtol(name+end,NULL,10);
}

int isblocked( claCfg_t* config, dev_ctrl_t* dev){

        int i;
        char letter;
        int number;
	
	if( isactive( config, OPT_ignore_block ) ) 
		return 0;
	if( check_substring( config, OPT_block_vendor, dev->vendor ) )
		return OPT_block_vendor;
	if(	check_substring( config, OPT_block_name,   dev->name ) )
		return OPT_block_name;
	if( check_substring( config, OPT_block_serial, dev->serial ) )
		return OPT_block_serial;
	if(	check_substring( config, OPT_block_bus, dev->bus_name ) )
		return OPT_block_bus;
	if( isactive(config, OPT_block_nonremovable) && !dev->removable )
		return OPT_block_nonremovable;
	if( isactive( config, OPT_block_device ) ){
		if( 
			check_substring( config, OPT_block_device, dev->vendor ) ||
			check_substring( config, OPT_block_device, dev->name ) ||
			check_substring( config, OPT_block_device, dev->serial )
		)
			return OPT_block_device;
	}

	if( isactive( config, OPT_block_letter ) ){
		letter = drive_letter( dev->user_name );
		if(letter)
			for(i=0;i<argnum(config,OPT_block_letter);i++){
				if(strchr(soptM(config,OPT_block_letter,i),tolower(letter)))
					return OPT_block_letter;
				if(strchr(soptM(config,OPT_block_letter,i),toupper(letter)))
					return OPT_block_letter;
			}
	}
	
	if( isactive( config, OPT_block_number ) ){
		number = drive_number( dev->user_name );
		if(number>=0)
			for(i=0;i<argnum(config,OPT_block_number);i++)
				if(number==noptM(config,OPT_block_number,i))
					return OPT_block_number;
	}
	
	return 0;

}

int prompt( dev_ctrl_t* dev, claCfg_t* config, const char* action_name ){

	char reply[MAX_PATH+1];
	int c;
	int result;
	int block_status = isblocked( config, dev );

	if( block_status ){
		printf( "\n!! Writing operations for %s is blocked by '%s' option !\n", 
			dev->user_name, opt[block_status].optName );
		return 0;
	}

	printf("\n\tSelected operation:\t %s\n",action_name);
	printf("\tSelected drive:\t\t %s, %I64db (%s)\n\n", dev->user_name, dev->size, human_view( dev->size, 'b') );

	if( isactive( config, OPT_do_not_confirm ) ){
		result = 0;
	}else{
		printf( "\tTHIS OPERATION IS DESTRUCTIVE!!!\n" );
		printf( "\tType 'yes' to confirm operation. All other text will stop it.\n\n");
		printf( "\tReally destroy data on drive %s?  :",dev->user_name);

		fgets( reply, MAX_PATH, stdin );

		for( c = 0; reply[c] && c < MAX_PATH; c++ )
			reply[c] = toupper( reply[c] );

		result = strncmp( reply, "YES\n", MAX_PATH );
		if(result)
			printf( "\nAction canceled\n" );
		printf("\n");
	}
	
	if(!result){
		print_header("[Log]");
		printf("Runing operation [%s] for drive %s\n\n", action_name, dev->user_name );
	}

	return !result;

}




/*global varible*/
dev_ctrl_t* selected_device;
int enter_key = 1;

void end_message(void){
	if( selected_device )
		print_footer( selected_device );
	if(enter_key)
		wait_for_key("\n\nPress ENTER to exit.         \n");
	title_restore();
}


static claCfg_t* read_config_file(){

	#ifdef CONFIG_FILE
		#ifdef HOME_DIR_CONFIG
			char real_config[MAX_PATH+1];
			char *home_dir=strdup(_pgmptr);
			int c;
			for( c=strlen(home_dir);c;c--){
				if(home_dir[c]=='\\'){
					home_dir[c+1]=0;
					break;
				}
			}
			strcpy(real_config,home_dir);
			strncat(real_config,CONFIG_FILE,MAX_PATH);
			free(home_dir);
			return ReadTextConfig( real_config, opt, CLA_FLAG_STOP_ALL_ERRORS );			
		#else
		return ReadTextConfig( CONFIG_FILE, opt, CLA_FLAG_STOP_ALL_ERRORS );
		#endif
	#else
	return NULL;
	#endif
}


int main( int argc, char* argv[] ){

	io_ctrl_t 	io_ctrl		=	{ 0, 0, 0, 0, 0, 0, 0, 0, stdout };
	claCfg_t*	config		=	NULL;
	claCfg_t*	command_line	=	NULL;
	claCfg_t*	config_file	=	NULL;
	dev_ctrl_t*	dev		=	NULL;
	int 		c		=	0;
	int		mode		=	0;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	atexit( &end_message );

	command_line =  ParseCommandLine(argc, argv, opt, 0, 1, CLA_FLAG_STOP_ALL_ERRORS );
	if(! command_line )
		exit( -1 );
	if( isactive( command_line, OPT_title ) )
		title_init();
	if( isactive ( command_line, OPT_ver ) ){
		printf( app_ver );
		enter_key =  0;
		claClose( command_line );
		return 0;
	}


	if( isactive( command_line, OPT_probe ) ){
		list_disks( nopt( command_line, OPT_probe ) );
		exit(0);
	}

	if( !freeoptnum( command_line ) || isactive( command_line, OPT_help ) ){
		printf( app_description );
		printf( app_ver );
		PrintOpt( opt, stdout );
		printf( copyright );
		if( !freeoptnum( command_line ) ){
			printf( dump_warning ); /*for mental-reduced users, expecting something after double-clicking on program*/
		}
		claClose( command_line );
		return 0;
	}

	
	config_file = read_config_file( );
	if( config_file ){
		config = claJoin( config_file, command_line, 2, 1 ); /*config file has a higher priority than a command line*/
		if( !config )
			exit( -1 );
	}else
		config = command_line;
	
	enter_key =! isactive( config, OPT_key );
	SetErrorMode( SEM_NOOPENFILEERRORBOX );

	mode= claGroupOpt( config, CLA_GROUP_0 );
	if(!mode){
		printf("No action selected\n");
		return 0;
	}

	io_ctrl.start 		= (long long)noptM( config, OPT_range,  0 );
	io_ctrl.end		= (long long)noptM( config, OPT_range,  1 );
	io_ctrl.ignore_error 	= nopt(  config, OPT_ignore );
	io_ctrl.block_size	= nopt(  config, OPT_block_size  );
	io_ctrl.write_share	= isactive( config, OPT_share );

	if(mode==OPT_test){
		mode=OPT_inc;
		io_ctrl.verify_mode=3;
		io_ctrl.mode=1;
		io_ctrl.delay=10;
	}

	if( isactive( config, OPT_verify ) ) 
		io_ctrl.verify_mode   = nopt(  config, OPT_verify );
	if( isactive( config, OPT_mode ) )
		io_ctrl.mode		  = nopt(  config, OPT_mode   );
	if( isactive( config, OPT_delay ) )
		io_ctrl.delay		  = nopt(  config, OPT_delay  );


//	if ( isactive( config, OPT_advanced ) ){
//		advanced_recovery( freeopt( config, 0 ), soptM( config, OPT_advanced, 0), soptM(config, OPT_advanced, 1), soptM(config, OPT_advanced, 2), &io_ctrl );
//		return 0;
//	}

	dev  = open_drive( freeopt( config, 0 ), 0, io_ctrl.write_share );
	
	if( !dev ){
		title_text2("Fail to open ",freeopt( config, 0 ));
		claClose( config );
		return 1;
	}

	get_drive_info( dev, 0xFF, 0 ); /*FIX*/
	title_dev_text ( "device info", dev );
	display_drive_info(dev);

	switch(mode){
		case OPT_blank:
			if( prompt( dev, config, "erase first sector" ) ){
				selected_device = dev;	
				blank( dev, &io_ctrl );
			}
			break;
		case OPT_fill:
			if( prompt( dev, config,  "fill sectors" ) ){
				selected_device = dev;	
				for( c = 0; c < (isactive( config, OPT_cycle )? nopt( config, OPT_cycle ) : 1); c++)
					fill( dev, nopt( config, OPT_fill ), &io_ctrl );
			}
			break;
		case OPT_inc:
			if( prompt( dev, config,  "write test" ) ){
				selected_device = dev;	
				for( c = 0; c < ( isactive( config, OPT_cycle )? nopt( config, OPT_cycle ) : 1); c++)
					inc_fill( dev, nopt( config, OPT_inc ), &io_ctrl );
			}
			break;
		case OPT_read:
			selected_device = dev;	
			read_test( dev, &io_ctrl );
			break;
		case OPT_quick:
			if( prompt( dev, config, "quick r/w test" ) ){
				selected_device = dev;
				quick_test( dev, &io_ctrl );
			}
			break;
		case OPT_update:
			if( prompt( dev, config, "update (rewrite)" ) ){
				selected_device = dev;
				update_action( dev, &io_ctrl );
			}
			break;
		case OPT_load:
			if( prompt( dev, config, "load file content" ) ){
				selected_device = dev;	
				load_image( dev, sopt( config, OPT_load ), &io_ctrl );
			}
			break;
		case OPT_save:
				selected_device = dev;	
				save_image( dev, sopt( config, OPT_save ), &io_ctrl );
			break;
		

	}
//	close_drive ( dev );
	claClose ( config );

	return 0;
}


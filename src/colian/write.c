#include "colian.h"
#include "internal.h"


int claWriteTextConfig(claCfg_t* cfg, const char* FileName, claOptName_t* OptList, int OptListNum, int Ctrl ){

	FILE* handle;
	int c;
	int i;
	int flag_saved_item=0;

	if( cfg->entryNum != OptListNum ) {
		if( !( Ctrl & CLA_FLAG_SILENCE ) ){
			printf( CLA_MSG_INCORRECT_CALL );
		}
		return -1;
	}

	if( !FileName ){
		if( !( Ctrl & CLA_FLAG_SILENCE ) )
			printf( CLA_MSG_NO_FILENAME );
		return -1;
	}

	handle = fopen( FileName, "w" );
	if( !handle ){
		if( !( Ctrl & CLA_FLAG_SILENCE ) )
			printf( CLA_MSG_IO_ERROR, FileName, strerror( errno ) );
		return -1;
	}

	if( Ctrl & CLA_FLAG_HEAD_INFO )
		fprintf( handle, CLA_MSG_TEXT_CONFIG_HEAD );

	for( c = 0; c < cfg->entryNum; c++ ){

		flag_saved_item = 0;

		if( cfg->item[c].found || ( Ctrl & CLA_FLAG_SAVE_ALL ) ){

			if( !( Ctrl & CLA_FLAG_IGNORE_NONSAVABLE ) ){

				if( !( OptList[c].optType & CLA_CFG ) ){

					if( !( Ctrl&CLA_FLAG_SILENCE ) )
						printf( CLA_MSG_NONSAVABLE, OptList[c].optName );

					if( !( Ctrl & CLA_FLAG_STOP_NONSAVABLE ) ){
						fclose( handle );
						/*add delete*/
						return 0;
					}

					continue;
				}
			}

			if( ( OptList[c].optType & CLA_CFG ) || (Ctrl & CLA_FLAG_SAVE_ALL) ){

				flag_saved_item = 1;
	
				fprintf( handle, "%s = ", OptList[c].optName ); /*add error checking*/

				for( i=0; i < cfg->item[c].argNum; i++ ){
	
					if( cfg->item[c].type & CLA_TYPE_STRING )
						fprintf( handle, "%s", cfg->item[c].slist[i] );
					else
						fprintf( handle, "%d", cfg->item[c].nlist[i]);

					if( i + 1 < cfg->item[c].argNum )
						fprintf( handle, ";" );

				}
				fprintf( handle, "\n" );
			}

		}else{
	
			if( ( (Ctrl & CLA_FLAG_SAVE_NOTFOUND) && ( OptList[c].optType & CLA_CFG ) ) || ( Ctrl & CLA_FLAG_SAVE_ALL ) ){

				flag_saved_item = 1;

				fprintf( handle, "#%s=", OptList[c].optName );

				if( OptList[c].optType & CLA_TYPE_STRING )
					fprintf( handle, " \n" );
				else
					fprintf( handle, "0\n" );

			}

		}

		if( flag_saved_item && ( Ctrl & CLA_FLAG_OPT_COMMENT ) ){
			fprintf( handle, "# %s: %s\n\n", OptList[c].optName, OptList[c].description );
		}

	}
		
	fclose( handle );

	return 1;

}


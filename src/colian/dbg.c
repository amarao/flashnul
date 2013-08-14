#include "colian.h"
#include "internal.h"

void dbg_print_cfg( FILE* f, claOptName_t* optlist, claCfg_t* cfg ){

	int c,j;

	if( !f && !cfg ) 
		return;
	for( c=0; c < cfg->entryNum; c++ ){
		if( cfg->item[c].found ){

			fprintf(
				f,
				CLA_MSG_DBG_BODY,
				optlist[c].optName,
				optlist[c].optType,
				optlist[c].optType & 0xF
			);
			for( j=0; j < cfg->item[c].argNum; j++ ){
				if( optlist[c].optType & CLA_TYPE_STRING )
					fprintf( f, "%s ", cfg->item[c].slist[j] );
				else 
					fprintf( f, "%d ", cfg->item[c].nlist[j] );
			}

			fprintf( f, "\n" );

		}
	}

	if( cfg->FreeOptNum ){

		fprintf( f, CLA_MSG_DBG_FREEOPT_HEAD, cfg->FreeOptNum );
		for( c = 0; c < cfg->FreeOptNum; c++ )
			fprintf( f, "%d = %s\n", c + 1, cfg->FreeOpt[c] );

	}

}

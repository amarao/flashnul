#include "colian.h"
#include "internal.h"

claCfg_t* claInit( int OptListNum, int maxFreeOpt ){

	claCfg_t* retval	=	calloc( 1, sizeof(claCfg_t) );
	retval->item		=	calloc( sizeof(claCfgItem_t), OptListNum  );
	retval->entryNum	=	OptListNum;
	retval->FreeOpt		=	calloc( sizeof(char*), maxFreeOpt );

	return retval;

}

void claClose( claCfg_t* c ){

	int i, j;

	if( !c )
		return;

	for( i=0; i<c->entryNum; i++ ){
		if( c->item[i].found) {
			if( c->item[i].type & CLA_TYPE_STRING ){
					for( j=0; j<c->item[i].argNum; j++ )
						free( c->item[i].slist[j] );
					free( c->item[i].slist );
			}
			else
				if(c->item[i].argNum) 
					free( c->item[i].nlist );
		}
	}

	for( i=0; i< c->FreeOptNum; i++ )
		free ( c->FreeOpt[i] );
	
	if( c->FreeOpt )
		free( c->FreeOpt );

	free( c->item );
	
	free( c );

}

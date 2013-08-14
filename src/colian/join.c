#include "colian.h"
#include "internal.h"

claCfg_t* claJoin(claCfg_t* config1, claCfg_t* config2, int Priority1, int Priority2){

	claCfg_t* source;
	claCfg_t* target;

	int c;

	if( !config1 && config2 )
		return config2;

	if( config2 && !config1 )
		return config1;

	if(!config1 && config2 )
		return NULL;

	if( config1->entryNum != config2->entryNum ) 
		return NULL;

	if( Priority2 >= Priority1 ){
		source = config1;
		target = config2;
	}else{
		source = config2;
		target = config1;
	}

	for( c=0; c < source->entryNum; c++ ){
		if( source->item[c].found && !target->item[c].found ){
			if( claGroupOpt( target, source->item[c].type  & CLA_GROUP_ALL ) < 0 ){
				target->item[c] = source->item[c];
				source->item[c].found = 0; /*disable record to awoid double free for strings*/
			}else{
			      /*todo: error handling*/
			}
		}
	}

	if( source->FreeOptNum ){

		target->FreeOpt = realloc(
			target->FreeOpt,
			sizeof( char* ) * ( target->FreeOptNum + source->FreeOptNum )
		);
		assert( target );

		for( c=0; c < source->FreeOptNum; c++ ){
			target->FreeOpt[target->FreeOptNum + c] = source->FreeOpt[c];
		}

		target->FreeOptNum += source->FreeOptNum;
		source->FreeOptNum = 0; /*to avoid double free to FreeOpt in claClose()*/

	}

	claClose( source );

	return target;

}

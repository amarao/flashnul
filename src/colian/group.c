#include "colian.h"
#include "internal.h"

/** Search for any option with specified in group_mask group(s),
(two or more groups can be combined with '|')**/

/**return value: -1 if nothing is found, or order number of found option**/

int claGroupOpt( claCfg_t* config, int group_mask ){

	int c;

	if( group_mask )
		for( c = 0; c < config->entryNum; c++ )
			if( config->item[c].found && ( config->item[c].type & group_mask ) )
				return c;

	return -1;

}


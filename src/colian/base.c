#include "colian.h"
#include "internal.h"
#include "i18n.h"

char* SkipSpaces( const char* line ){

	int i = 0;

	while( line[i] == ' ' || line[i] == '\t' ) 
		i++;

	return (char*)( line + i );
}

int OptNameCmp( const char* sample, const char* name, int case_ignore_flag ){
	/*return:
		<0 - error in comparing
		>1 - compare ok, return value points to end of name (e.g. to '=' or ':')
		0  - compare ok, no '=' or ':' at the end*/

	int c = 0;

	if( case_ignore_flag ){
		while( name[c] ){
			if( toupper( sample[c] ) != toupper( name[c] ))
				return -1;
			c++;
		}
	}else{
		while( name[c] ){
			if( sample[c] != name[c] ) 
				return -1;
			c++;
		}
	}

	if( sample[c] == ':' || sample[c] == '=' ) 
		return c;

	if( !sample[c] || sample[c]==' ' || sample[c]=='\t') 
		return 0;

	return -1;

}



char* SkipEquiv(const char* line){

	char* c = SkipSpaces(line);

	if( *c != '=' )
		return "";

	c++;

	return SkipSpaces(c);

}



/** 
 * return order number of long option, or -1 if option not found.
 * valid OptList array requied
**/



int GetOptionID(
		const char* line,
		const claOptName_t* OptList,
		const int OptListNum,
		int type
){

	int c;
	int res;
	for( c=0; c < OptListNum; c++ ){
		if( *(OptList[c].optName) ){

			res = OptNameCmp( line, OptList[c].optName, OptList[c].optType & CLA_CASE_INSENSITIVE );

			if( res<0 ) 
				continue;

			if( !(OptList[c].optType&type) ) 
				continue;

			return c;

		}
	}

	return -1;

}

char* SplitLine( const char* line ){

	char* return_string = NULL;
	int counter = 0;

	while( line[counter] && line[counter] != ';' )
		counter++;

	return_string = malloc( counter + 1 );

	if( counter )
		memcpy( return_string, line, counter );

	return_string[ counter ] = 0;

	return return_string;

}

int SetArguments( const char* line, claOptName_t* optN, claCfgItem_t* optV ){
/** parse line for arguments, option description passed in OptN, place for arguments passed in optV.

	return value:

		-1 - all ok
		-3 - not enough args
		-4 - too much args
		-5 - args not found
		-6	- incorrect value/type
**/
	


	int separators	=	0;
	int arguments	=	0;
	int c;
	int args_type	=	0;
	char* argument	=	NULL;
	int arg_offset	=	0;
	int j;
	int retcode	=	-1;

	for( c=0; line[c]; c++ )
		if( line[c] == ';' ) 
			separators++;

	if( !separators )
		if( !*line )
			return -5;

	arguments = separators + 1;

	args_type = optN->optType & ( CLA_TYPE_NUMBER | CLA_TYPE_STRING );
	
	switch(args_type){

		case CLA_TYPE_NUMBER:
			optV->nlist = realloc( optV->nlist, optV->argNum+arguments * sizeof(CLA_INT*) );
			assert( optV->nlist );
			for( j = 0; j < arguments; j++ )
				optV->nlist[ j + optV->argNum ] = 0;
		break;

    	case CLA_TYPE_STRING:
			optV->slist = realloc( optV->slist, optV->argNum+arguments * sizeof(char**) );
			assert(optV->slist);
			for(j=0;j<arguments;j++)
				optV->slist[j+optV->argNum]=NULL;
		break;

		default: 
			return -6;

	}

	for(c=0;c<arguments;c++){

		argument = SplitLine( line + arg_offset );
		assert( argument );
		arg_offset += strlen( argument ) + 1;

		switch(args_type){
			case CLA_TYPE_NUMBER:
				for(j=0;argument[j];j++)
					if(!isdigit(argument[j])&&!isspace(argument[j]))
						retcode = -6;
				optV->nlist[optV->argNum++] = atoll( argument );
				free( argument );
			break;
			case CLA_TYPE_STRING:
				optV->slist[optV->argNum++] = argument;
			break;
			default:
				free( argument );

		}

	}
	optV->found++;
	optV->type = optN->optType;
	if(optV->argNum<optN->minAllowed)
		retcode = -3;
	if(optV->argNum>optN->maxAllowed && optN->maxAllowed)
		retcode = -4;

	return retcode;
}


int LineToOpt( claCfg_t* cfg,claOptName_t* opt, int optn, char* parse_line, int area ){
/*thesises: used ONLY for readCfg/ReadVarEnv.*/
/* retcode:
		-2 unknown option
		-3 not enough arguments for option
		-4 too many arguments
		-5 option requed argument
		-6 incorrect value
		-1 - success (!!!)
		>=0 - number of conflict option
*/

		int ID;
		char* argument=NULL;
		int conflict_ID=-1;
		int paramflag=0;

		ID = GetOptionID( parse_line, opt, optn, area );

		if( ID < 0 )
			return -2;

		conflict_ID = claGroupOpt(cfg,opt[ID].optType &  CLA_GROUP_ALL );
		if( conflict_ID >= 0 && conflict_ID != ID ) /*confict found*/
			return conflict_ID;

		if( opt[ID].ID != ID ){
			printf(CLA_MSG_BAD_DECLARATION,opt[ID].optName);
			exit(-1); /*todo???*/
		}
	
		cfg->item[ID].ID = opt[ID].ID;
		cfg->item[ID].type = opt[ID].optType;

		if(opt[ID].optType & CLA_TYPE_SWITCH){
			cfg->item[ID].nlist=realloc(cfg->item[ID].nlist,sizeof(CLA_INT));
	        assert(cfg->item[ID].nlist);
			cfg->item[ID].nlist[0]++;
			cfg->item[ID].found++;
			return -1;/*ok*/
		}

		argument = SkipEquiv( parse_line + strlen( opt[ID].optName ) );
		return SetArguments( argument, &(opt[ID]), &(cfg->item[ID]) ) ;
}

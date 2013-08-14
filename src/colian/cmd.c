#include "colian.h"
#include "internal.h"

int GetOptionID_short(const char letter,claOptName_t* OptList,const int OptListNum){
	/*return a ID of option (by short name), or -1 if option not found*/
	int c;
	
	for( c = 0; c < OptListNum; c++ )
		if( OptList[c].optLetter == letter && ( OptList[c].optType & CLA_CMD ) )
			return c;

	return -1;
}


static void SetFreeOpt (claCfg_t* cfg, char* line){
	cfg->FreeOptNum++;
	cfg->FreeOpt = realloc( cfg->FreeOpt, cfg->FreeOptNum * sizeof(char*) );
	assert( cfg->FreeOpt );
	cfg->FreeOpt[cfg->FreeOptNum - 1] = line;
}


claCfg_t* claParseCommandLine( 
	int argc, char* argv[], claOptName_t* OptList, int OptListNum, 
	int MinFreeNum, int MaxFreeNum, int Ctrl 
){

	claCfg_t* retval	=	claInit(OptListNum,MaxFreeNum);

	int		c			=	0;
	int		j			=	0;

	int		args_left	=	0; /*number of arguments to parse*/
	int		ID			=	-1;
	int 	conflict_ID =	-1;
	int 	result_code = 	0;
	int		silence_flag=	Ctrl & CLA_FLAG_SILENCE;

	assert( retval );

	for( c = 1; c < argc; c++ ){


		if(args_left > 0){/*pickup leftover arguments for option*/
			if( ID < 0 ){
				if( !silence_flag )
					printf(CLA_MSG_CMD_INTERNAL_ERROR_1);
				claClose(retval);
				return NULL;
			}
			args_left=0;
			result_code = SetArguments( argv[c], &( OptList[ID] ), &( retval->item[ID] ) );
			switch( result_code ){
				case -1: /*ok*/
					continue;
				case -3: /*not enough*/
					args_left = 1;
					continue;
				case -4: /*too much*/
					if( !silence_flag )
						printf( CLA_MSG_CMD_TOO_MUCH_SL,OptList[ID].optName, OptList[ID].optLetter);
					if(Ctrl&CLA_FLAG_STOP_TOO_MUCH){
						claClose(retval);
						return NULL;
					}
					continue;
				case -5:/*argument not found - impossible here (i.e. argv[c] can not be empty string) */
					continue;
				case -6: /*invalid argument(s) type*/
					if(!silence_flag)
						printf(CLA_MSG_CMD_INCORRECT_ARG_SL,OptList[ID].optName, OptList[ID].optLetter, argv[c]);
					if( Ctrl & CLA_FLAG_STOP_INCORRECT_ARG ){
						claClose(retval);
						return NULL;
					}
			}
			continue;
		}


		if(argv[c][0]=='-'){

			if( !argv[c][1] ){ /*  single '-' is a Free Option */
				SetFreeOpt( retval, "-" );
				continue;
			}

			if(argv[c][1]=='-'){ 

				if(!argv[c][2]){/*stop mode, all other options should be accepted as free option*/
					for( j = c+1 ; j < argc ; j++ )
						SetFreeOpt( retval, argv[j] );
					break;
				}

				/*long option*/
				
				result_code = LineToOpt(retval,OptList, OptListNum, argv[c]+2, CLA_CMD );
				if(result_code >= 0){
					if(!silence_flag)
						printf(CLA_MSG_CMD_CONFLICT_L,argv[c]+2,OptList[result_code].optName);
					if( Ctrl & CLA_FLAG_STOP_CONFLICT ){
						claClose(retval);
						return NULL;
					}
				}
				if(result_code < -1){/*result_code==-1 ok (skip). result_code < -1 errors*/
					switch(result_code){
						case -2: /*unknown*/
							if( !silence_flag )
								printf( CLA_MSG_CMD_UNKNOWN_L, argv[c]+2 );
							if( Ctrl & CLA_FLAG_STOP_UNKNOWN ){
								claClose( retval );
								return NULL;
							}
							continue;
						case -3: 
							/*passtrough*/
						case -5:/*not enough, not error, continue parsing in upper section */
						
							ID = GetOptionID( argv[c] + 2, OptList, OptListNum, CLA_CMD );
							args_left = 1; 
							continue;
						case -4:/*too much*/
							if( !silence_flag )
								printf( CLA_MSG_CMD_TOO_MUCH_L, argv[c] + 2 );
							if( Ctrl & CLA_FLAG_STOP_TOO_MUCH ){
								claClose( retval );
								return NULL;
							}
							continue;
						case -6: /*invalid argument type*/
							if( !silence_flag )
								printf( CLA_MSG_CMD_INCORRECT_ARGS_L, argv[c] + 2);
							if( Ctrl & CLA_FLAG_STOP_INCORRECT_ARG ){
								claClose( retval );
								return NULL;
							}
					}
				}
				ID=-1;
				args_left = 0; 
				continue;
			}

			/*short options*/
			for( j = 1; argv[c][j] && j; j++ ){

				ID = GetOptionID_short( argv[c][j], OptList, OptListNum );

				if( ID < 0 ){/*unknown option*/
					if( !silence_flag )
						printf(CLA_MSG_CMD_UNKNOWN_S, argv[c][j] );
					if( Ctrl & CLA_FLAG_STOP_UNKNOWN ){
						claClose( retval );
						return NULL;
					}
					continue;
				}

				if( OptList[ID].ID != ID ){
						printf(CLA_MSG_BAD_DECLARATION, OptList[ID].optName );
						exit(-1);
				}

				conflict_ID = claGroupOpt( retval, OptList[ID].optType & CLA_GROUP_ALL );
				if( conflict_ID >= 0 && conflict_ID != ID ){ 
					if( !silence_flag )
						printf( CLA_MSG_CMD_CONFLICT_S, argv[c][j], OptList[conflict_ID].optLetter );
					if( Ctrl & CLA_FLAG_STOP_CONFLICT ){
						claClose( retval );
						return NULL;
					}
					if(argv[c][j+1]==':'||argv[c][j+1]=='=') /*if -O=something, stop parsing*/
						j=-1;
					continue;
				}

				if(argv[c][j+1]=='='||argv[c][j+1]==':'){/*found argument(s) for shot option*/

					result_code = SetArguments(argv[c]+j+2, &(OptList[ID]), &(retval->item[ID]));

					switch( result_code){

						case -1: /*ok*/
							retval->item[ID].type=OptList[ID].optType;
							retval->item[ID].found++;
							args_left = 0;
							ID = -1;
							break;
						case -3: /*not enough, not error, just continue in upper section*/
							retval->item[ID].type=OptList[ID].optType;
							retval->item[ID].found++;
							args_left = 1;
							break;
						case -4:/*too much*/
							if( !silence_flag )
								printf( CLA_MSG_CMD_TOO_MUCH_S, argv[c][j] );
							if( Ctrl & CLA_FLAG_STOP_TOO_MUCH ){
								claClose( retval );
								return NULL;
							}
							break;
						case -5: /*not found*/ 
							if( !silence_flag )
								printf( CLA_MSG_CMD_ARG_NOT_FOUND_S, argv[c][j] );
							if( Ctrl & CLA_FLAG_STOP_UNKNOWN ){
								claClose( retval );
								return NULL;
							}
							break;
						case -6:/*invalid type*/
							if( !silence_flag)
								printf( CLA_MSG_CMD_INCORRECT_S, argv[c][j] );
							if( Ctrl & CLA_FLAG_STOP_INCORRECT_ARG ){
								claClose( retval );
								return NULL;
							}
							break;
					}
					break;

				}else{/*option without '='*/
					if( OptList[ID].optType & CLA_TYPE_SWITCH ){
						retval->item[ID].argNum++;
						retval->item[ID].nlist = realloc (retval->item[ID].nlist,retval->item[ID].argNum );
						assert( retval->item[ID].nlist );
						retval->item[ID].nlist [retval->item[ID].argNum-1]++;
						retval->item[ID].type = OptList[ID].optType;
						retval->item[ID].found++;
						continue;
					}
					if(!OptList[ID].minAllowed){
						retval->item[ID].found++;
						retval->item[ID].type=OptList[ID].optType;
						retval->item[ID].argNum=1;
						if(OptList[ID].optType & CLA_TYPE_STRING){
							retval->item[ID].slist=malloc(sizeof(char*));
							assert(retval->item[ID].slist);
							retval->item[ID].slist[0]=strdup("");
							assert(retval->item[ID].slist[0]);
						}
						if(OptList[ID].optType & CLA_TYPE_NUMBER){
							retval->item[ID].nlist=malloc(sizeof(CLA_INT));
							assert(retval->item[ID].nlist);
							retval->item[ID].slist[0]=0;
						}
						continue;
					}
					if( !argv[c][j+1] ){ /*ok, continue argument parsing*/
						args_left =1;
						break;
					}else{ /*error*/ /*TODO args accepting?*/
						if( !silence_flag)
							printf(CLA_MSG_CMD_ARGS_REQUED_S,argv[c][j]);
						if( Ctrl & CLA_FLAG_STOP_NOT_ENOUGH ){
							claClose( retval );
							return NULL;
						}
					}
				}
			}
			continue;
		}
		/*free opt*/
		SetFreeOpt( retval, argv[c] );
	}
	if(args_left && ID>=0){
		if(!silence_flag)
			printf(CLA_MSG_CMD_NOT_ENOUGH_FREE,OptList[ID].optName, OptList[ID].optLetter);
		if(Ctrl&CLA_FLAG_STOP_NOT_ENOUGH){
			claClose(retval);
			return NULL;
		}
	}

	return retval;
}

#include "colian.h"
#include "internal.h"
#define DEFAULT_LINE_BUFFER_SIZE 128

/**
 * read line from text file 
 * behavior is not defined if file contains a zero (0x00) character
 * return number of readed characterss or zero, if no characters has been 
 * readed (EOF or i/o error)
 * put readed string to *target, size is keeping in *target_size.
 * if ** target is a NULL, *target contain a dynamically allocated string
 * (independ on success of reading).
 * return string allways ending by 0-character.

**/


int GetLine( FILE* from, char** target, int *target_size ){

#if DEFAULT_LINE_BUFFER_SIZE < 2
    #error DEFAULT_LINE_BUFFER_SIZE must be >= 2
#endif

    int     block_size = DEFAULT_LINE_BUFFER_SIZE;
    int     offset     = 0;

	if( !*target ){
		*target = malloc( DEFAULT_LINE_BUFFER_SIZE );
		assert( *target );
		(*target)[0] = 0;
		*target_size = DEFAULT_LINE_BUFFER_SIZE;
	}

    while( fgets( *target + offset, block_size, from ) ){
        offset += strlen( *target + offset );

        if( offset < 2 ) 
            break;

        if( (*target)[offset - 1] == '\n' || (*target)[offset - 1] == '\r' )    
            break;

        block_size += block_size/2;
        *target_size += block_size;
        *target = realloc( *target, *target_size );
        assert( *target );
    }

    return offset;
}

void CropComment( char* line ){
	char* p=line;
	while(*p)
		if(*p=='#'){
			*p=0;
			break;
		}
}


void CropCommentAndCRLF(char* line){
	int i;
	for(i=0;line[i];i++){
		if(line[i]=='\n'||line[i]=='\t'||line[i]=='#'){
			line[i]=0;
			return;
		}
	}
}




int CheckInclude( const char* line){
	/*check for 'include' word and return offset to start of word after include. Return 0 if no 'include' found*/
	unsigned int include_len = strlen( CLA_KEYWORD_INCLUDE );
	char* p = SkipSpaces( line );

	if( p ){
		if( strlen( p ) > include_len ){
			if(!memcmp(CLA_KEYWORD_INCLUDE,p,include_len)){
				if(p[include_len]==' '||p[include_len]=='\t') 
					return (int)(SkipSpaces(p+include_len)-line);
			}
		}
	}
	return 0;
}


claCfg_t* claReadTextConfig( const char* FileName, claOptName_t* OptList, int OptListNum, int Ctrl ){

	claCfg_t*	retval;
	FILE*		handle;
	char*		buf				=	NULL;
	int 		buf_size		=	0;
	char*		line;
	int 		line_counter	=	0;
	int 		include_position=	0;
	int			silence_flag 	= 	Ctrl & CLA_FLAG_SILENCE;
	int			result_code		=	0;

	struct {
		int		flag;
		char*	msg;
	}read_error_act[]={
		{	0,								NULL						},	/*skip*/
		{	0,								NULL						},	/*skip*/
		{	CLA_FLAG_STOP_UNKNOWN,			CLA_MSG_FILE_UNKNOWN_OPTION },	/*unknown option*/
		{	CLA_FLAG_STOP_NOT_ENOUGH,		CLA_MSG_FILE_NO_ENOUGH_ARGS	},	/*not enough arguments*/
		{	CLA_FLAG_STOP_TOO_MUCH,			CLA_MSG_FILE_TOO_MUCH_ARGS	},	/*too many arguments*/
		{	CLA_FLAG_STOP_OPTION_REQUED,	CLA_MSG_FILE_OPTION_REQUED	},	/*no eny arguments found*/
		{	CLA_FLAG_STOP_INCORRECT_ARG,	CLA_MSG_FILE_INCORRECT_ARG	},	/*incorrect argument type*/
	};


	if( !FileName ){
		if( !silence_flag )
			printf( CLA_MSG_NO_FILENAME );
		return NULL;
	}

	handle = fopen( FileName, "r" );
	if( !handle ){
		if(!silence_flag)
			printf( CLA_MSG_IO_ERROR, FileName, strerror(errno) );
		return NULL;
	}

	retval = claInit( OptListNum, 0 );	

	assert( retval );

	while( GetLine( handle, &buf, &buf_size ) ){

 		line_counter++;

		CropCommentAndCRLF( buf );

		line = SkipSpaces( buf );
		if( !*line ) 
			continue;

		if( (Ctrl&CLA_FLAG_ENABLE_INCLUDE) ){
			include_position = CheckInclude( line );
			if( include_position ){
				if( !strcmp( FileName, line + include_position ) ){
					if(!silence_flag)
						printf(CLA_MSG_SELF_RECURSION, FileName,line_counter);
					continue;
				}
				retval = claJoin(  /*joining current config and included (priority to included) */
					retval,
					claReadTextConfig( line + include_position, OptList, OptListNum, Ctrl ),
					0,1 
				); 
				continue;
			}
		}

		result_code = LineToOpt( retval, OptList, OptListNum, line, CLA_CFG );

		if( result_code >= 0 ){/*error handling, conflict option*/
			if(! silence_flag)
				printf(CLA_MSG_FILE_CONFLICT, FileName, line_counter, line, OptList[result_code].optName  );
			if( Ctrl & CLA_FLAG_STOP_CONFLICT ){	
				claClose( retval );
				fclose( handle );
				free( buf );
				return NULL;
			}
			continue;
		}

		if( result_code < -1 ){/*error handling, notfound/arguments, etc*/
			assert( result_code >= -6 );
			if(!silence_flag)
				printf( read_error_act[ 0 - result_code ].msg, FileName, line_counter, line );
			if(Ctrl & read_error_act[ 0 - result_code ].flag ){
				fclose( handle );
				claClose( retval );
				free( buf );
				return NULL;
			}
			continue;
		}
	}
	fclose(handle);
	free( buf );
	return retval;

}

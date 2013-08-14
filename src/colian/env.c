#include "colian.h"
#include "internal.h"

claCfg_t* claReadEnvironment( char **envp,claOptName_t* option, int option_number,int Ctrl ){

	claCfg_t*	retval	= 	claInit ( option_number,0);	
	int 		ec 		= 	0;
	int 		silence_flag = Ctrl & CLA_FLAG_SILENCE;
	int			result_code = 0;

	struct {
			int	flag;						char* msg;
	}env_error_act[]={
		{	0,								NULL						},	/*skip*/
		{	0,								NULL						},	/*skip*/
		{	0,								0							},	/*skip*/
		{	CLA_FLAG_STOP_NOT_ENOUGH,		CLA_MSG_ENV_NO_ENOUGH_ARGS	},	/*not enough arguments*/
		{	CLA_FLAG_STOP_TOO_MUCH,			CLA_MSG_ENV_TOO_MUCH_ARGS	},	/*too many arguments*/
		{	CLA_FLAG_STOP_OPTION_REQUED,	CLA_MSG_ENV_OPTION_REQUED	},	/*no eny arguments found*/
		{	CLA_FLAG_STOP_INCORRECT_ARG,	CLA_MSG_ENV_INCORRECT_ARG	},	/*incorrect argument type*/
	};



	for(ec=0;envp[ec];ec++){

		result_code = LineToOpt( retval, option, option_number, envp[ec], CLA_ENV );
		if( result_code >= 0 ){/*error handling, conflict option*/
			if(! silence_flag)
				printf(CLA_MSG_ENV_CONFLICT, envp[ec], option[result_code].optName  );
			if( Ctrl & CLA_FLAG_STOP_CONFLICT ){	
				claClose( retval );
				return NULL;
			}
			continue;
		}


		if( result_code == -1 ) 
			continue; /*unknown options in environment just ignoring*/

		if( result_code < -2 ){/*error handling etc*/
			assert( result_code >= -6 );
			if( !silence_flag )
				printf( env_error_act[ 0 - result_code ].msg, envp[ec]);
			if(Ctrl & env_error_act[ 0 - result_code ].flag ){
				claClose( retval );
				return NULL;
			}
			continue;
		}
	}
	return retval;

}

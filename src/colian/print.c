#include "colian.h"
#include "internal.h"

void claPrintOpt( claOptName_t* OptList, int OptListN, FILE* to ){

	int  c;
	int  lopt_size = 1;
	int  tmp;
	char lopt_print_tpl [128];

	if( OptList && OptListN ){

		for( c=0; c < OptListN; c++ ){
			if( OptList[c].optName[0] && (OptList[c].optType & CLA_CMD ) ){
				tmp = strlen( OptList[c].optName );
				if( tmp > lopt_size ) lopt_size = tmp;
			}
		}

		if( lopt_size > 50 ) 
			lopt_size = 50; /*TODO error message here?*/

		sprintf( lopt_print_tpl, "--%%-%ds", lopt_size + 1 );

		for( c=0; c < OptListN; c++ ){

			if( ! ( OptList[c].optType & CLA_CMD ) )
				continue;

			if( OptList[c].optLetter ) 
				fprintf( to, "-%c, ", OptList[c].optLetter );
			else 
				fprintf( to, "    " );
            if( OptList[c].optName[0] ) 
                fprintf( to, lopt_print_tpl, OptList[c].optName );
            else
                fprintf( to, "                       " );
            if( OptList[c].description )                
			    fprintf( to, "%s", OptList[c].description );
            fprintf( to, "\n" );

		}
	}

}
     

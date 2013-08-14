#ifndef _H_COLIAN_H
#define _H_COLIAN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include "clatypes.h"
#include "i18n.h"

/**

	* Section 3 *

    Function prototypes

**/


/**
	This function parse command line and return structure with parsed values.
	It returns pointer to structure if parsing successfull, return NULL otherwise.

	See also:  macross ParseCommandLine

	Note: returned value should be passed to claJoin() or closed by claClose()
		after use to avoid memory leak

**/
claCfg_t* claParseCommandLine(
	int 			argc, 		/** Number of application arguments in
									command line **/
	char* 			argv[],		/** Array of pointers to arguments **/
	claOptName_t* 	OptList, 	/** array of application options **/
	int 			OptListNum, /** Number of entries in OptList **/
	int 			MinFreeNum, /** minimal requed number of FreeOptions,
									(inoring, if option with CLA_EXCLUSIVE
									flag happens in arguments **/
	int 			MaxFreeNum, /** Maximal allowed number of FreeOptions **/
	int 			Ctrl		/** Bitfield for different behavior,
									see section 4 for possible values **/
);



/**
	This function parse text configuration file and return structure with parsed 
	values.	It returns pointer to structure if parsing successfull, return NULL 
	otherwise.

	See also:  macross ReadTextConfig

	Note: returned value should be passed to claJoin() or closed by claClose()
		after use to avoid memory leak

	Note3: This function allow to use keyword 'include' in configuration file,
	but check for self-reccurent call is very limited, so, for stability, disable
	include keyword by CLA_FLAG_DISABLE_INCLUDE bit in Ctrl bitfield.

**/
claCfg_t* claReadTextConfig( 
	const char*		FileName,	/** File name. Must be absolute or 
									relative path **/
	claOptName_t*	OptList,  	/** aray of apliation options **/
	int 			OptListNum, /** Number of entries in OptList **/
	int 			Ctrl		/** Bitfield for different behavior,
									see section 4 for possible values **/
);


/**
	This function join two configuration in one (them both MUST have a one
	(or identical) OptList! ). Result configuration contain values from
	both incoming configuration, in case of conflict, selecting depends on
	priority1 and priority2.

	Complete list of behavior (if no groups defined, else see notes):

	for each entry in both incoming configuration (config1, config2):

		if no active value in config1, config2, result has no active value.
		if only one from config1, config2 has active value, result equal to 
			it.
		if both config1 and config2 has active values, result depend on
			priority1 and priority2:
			if priority1 >= priority2, result equal to value from config1
			if priority2 <  priirity2, result equal to value from config2.


	Note1: after join you should not use config1 or config2 values, becouse
		claJoin clear 'em both

	Note: returned value should be passed to claJoin() or closed by claClose()
		after use to avoid memory leak

	Note: 
		some options from configuration with lower priority can be not 
		included to target configuration due group conflict with existing 
		options in configuration with higher priority.
**/

claCfg_t* claJoin(
	claCfg_t* 		config1, 	/** Fist config **/
	claCfg_t* 		config2, 	/** Second config **/
	int 			Priority1, 	/** Priority of first config **/
	int 			Priority2	/** Priority of second config **/
);


/**
	This function write configuration to text file.
	By default writing only active options with CLA_CFG flag
	(can be changed by CLA_FLAGS_* via Ctrl, see Section 4)

	return value:
		1 success
		-1 write fail due i/o error (error code keeps in errorlevel)
		0 error in configuration

**/
int claWriteTextConfig(
	claCfg_t* 		cfg, /** configuration for writing, must match OptList **/
	const char* 	FileName, /** Filename of file (if file exist, it will be replaced **/
	claOptName_t* 	OptList,  /** List of option name **/
	int 			OptListNum, /** number of entries of OptList **/
	int 			Ctrl  /** Control flags, see section 4 for possible values **/
);


/**
	Close configuration, free all allocated memory.
	Can cause undefined behavior, if config used after
	calling claClose or some of config data has been
	freed before call.
	return value: none
**/
void claClose(claCfg_t* config);


/**
	print to specified file (or stdin/stderr) list of option(s) from OptList,
	with description.
	printed option must have flag CLA_CMD (all others ignoring)

	return value:
	-1 - i/o error (error code keeps in errorlevel)
	0 - success
**/

void claPrintOpt(
	claOptName_t* 	OptList,
	int 			OptListN, 
	FILE* 			to
);


/** 
	Search for any option with specified in group_mask group(s),
	(two or more groups can be combined with '|')
	return value: 
		-1 if nothing is found, 
		order number of found option
**/

int claGroupOpt( claCfg_t* config, int group_mask );




/** those function used by macrosses sopt(M) nopt(M) below **/


/** debug print of options **/
void dbg_print_cfg(
	FILE* 			f, 
	claOptName_t* 	optlist, 
	claCfg_t* 		cfg
);

claCfg_t* claReadEnvironment( char **envp,claOptName_t* option, int option_number,int Ctrl );



/** This macross used instead claParseCommandLine to reduce number of arguments,
	instead OptList/OptListNum it accept a single option OptList, wich one MUST
	BE A ARRAY claOptName_t[].
**/
#define ParseCommandLine(argc, argv, OptList, MinFreeNum, MaxFreeNum, Error) \
	claParseCommandLine(argc,argv,OptList,sizeof(OptList)/sizeof(OptList[0]),MinFreeNum,MaxFreeNum,Error)


/** This macross used instead claReadTextConfig to reduce number of arguments,
	instead OptList/OptListNum it accept a single option OptList, wich one MUST
	BE A ARRAY claOptName_t[].
**/
#define ReadTextConfig(name, OptList, Ctrl) \
	claReadTextConfig(name, OptList, sizeof(OptList)/sizeof(OptList[0]),   Ctrl )

/** This macross used instead claWriteTextConfig to reduce number of arguments,
	instead OptList/OptListNum it accept a single option OptList, wich one MUST
	BE A ARRAY claOptName_t[].
**/
#define WriteTextConfig(cfg,FileName,opt,Ctrl) \
	claWriteTextConfig(cfg,FileName, opt, sizeof(opt)/sizeof(opt[0]), Ctrl )


#define ReadEnvironment(envp,option,Ctrl) \
	claReadEnvironment(envp,option,sizeof(option)/sizeof(option[0]),Ctrl)

/** This macross used instead claPrintOpt to reduce number of arguments,
	instead OptList/OptListNum it accept a single option OptList, wich one MUST
	BE A ARRAY claOptName_t[].
**/
#define PrintOpt(optlist,to) \
	claPrintOpt(optlist,sizeof(optlist)/sizeof(optlist[0]),to)


/** this macross return a number of Free Option**/
#define freeoptnum(cfg) (cfg->FreeOptNum)

/** This macross return specified Free Option **/
#define freeopt(cfg,OptN) (OptN < cfg->FreeOptNum ? cfg->FreeOpt[OptN] : "")





/** 
	nopt(cfg,ID) is a macross, returns value of the first numeric argument of 
	the option	with specified ID from specified configuration cfg, or 
	return 0 if option not found or non active. (and 0 can be returned as 
	valid value of active option). 
	Macross MUST be called with valid cfg structure.
**/
#define nopt(cfg,ID) \
	(ID<cfg->entryNum?( cfg->item[ID].found ? cfg->item[ID].nlist[0] : 0 ):0)


/** 
	sopt(cfg,ID) is a macross, returns pointer to the first string argument of
	the option	with specified ID from specified configuration cfg, or 
	return NULL if option not found or non active. (NULL never returns for
	valid active option). 
	Macross MUST be called with valid cfg structure.
**/
#define sopt(cfg,ID) \
	(ID<cfg->entryNum?( cfg->item[ID].found ? cfg->item[ID].slist[0] : ""):"")
#define stropt(cfg,ID)



/** 
	noptM(cfg,ID,order) is a macross, returns numeric value of the specifed by 
	order numeric argument of the option with specified ID from specified 
	configuration cfg, or return 0 if option not found or non active. 
	(and 0 can be returned as valid value of active option). 
	Macross MUST be called with valid cfg structure.
**/
#define noptM(cfg,ID,order)\
	(ID<cfg->entryNum?(\
	 cfg->item[ID].found ? (\
		order < cfg->item[ID].argNum?cfg->item[ID].nlist[order] : 0 \
	):0 \
):0)


/** 
	noptM(cfg,ID,order) is a macross, returns string value of the specifed 
	by order numeric argument of the option	with specified ID from specified 
	configuration cfg, or return NUKK if option not found or non active. 
	(NULL never returns for	valid active option). 
	Macross MUST be called with valid cfg structure.
**/
#define soptM(cfg,ID,order)\
	(ID<cfg->entryNum?(\
	 cfg->item[ID].found ? (\
		order < cfg->item[ID].argNum?cfg->item[ID].slist[order] : ""\
	):"" \
):"" )

/** 
	claOptNumber return number of arguments for specified by ID option 
	from specified configuration cfg or return 0 if option not found
**/
#define argnum( cfg, ID ) \
	(ID<cfg->entryNum?(cfg->item[ID].found?cfg->item[ID].argNum:0):0)



#define isactive( cfg, ID ) \
	(ID<cfg->entryNum?(cfg->item[ID].found):0)

/** 

	* Section 4 *

    Flags for Ctrl control bitfield

	Note: all those values are using for control function via Ctrl parameter.

**/



#define CLA_FLAG_STOP_UNKNOWN 0x2
	/** Stop parsing if unknown option found, function 
		will return a fail (NULL)
		Applicable to:
			claParseCommandLine
			claReadTextConfig
	**/

#define CLA_FLAG_STOP_NOT_ENOUGH 0x4
	/** Stop parsing, if no enough arguments for option found
		Applicable to:
			claParseCommandLine
			claReadTextConfig
	**/
	
#define CLA_FLAG_STOP_TOO_MUCH 0x8
	/** Stop parsing if too much arguments has been specified
		Applicable to:
			claParseCommandLine
			claReadTextConfig
	**/

#define CLA_FLAG_SILENCE 0x10
	/** Do not report anything to user (do not print anything to screen)
		Applicable to:
			claParseCommandLine
			claReadTextConfig
			claWriteTextConfig
	**/

#define CLA_FLAG_STOP_INCORRECT_ARG 0x20
	/** Stop parsing if incorrect type of argument (e.g. string instead number)
		Applicable to:
			claParseCommandLine
			claReadTextConfig
			claReadEnvironmentVar
	**/
	

#define CLA_FLAG_STOP_CONFLICT 0x40


#define CLA_FLAG_STOP_OPTION_REQUED 0x80

#define CLA_FLAG_STOP_ALL_ERRORS (CLA_FLAG_STOP_UNKNOWN|CLA_FLAG_STOP_NOT_ENOUGH|CLA_FLAG_STOP_TOO_MUCH|CLA_FLAG_STOP_INCORRECT_ARG|CLA_FLAG_STOP_CONFLICT|CLA_FLAG_STOP_OPTION_REQUED)

#define CLA_FLAG_ENABLE_INCLUDE 0x1000000
	/** Allow use keyword 'include', without this options including files 
		will be ignored, and line with include will be parsed as
		common line (and cause an error),
		Applicable only to claReadTextConfig.
	**/


#define CLA_FLAG_SAVE_ALL 0x1000000
	/** Save all active options, regardless CLA_CFG bit for optType(type).
		Applicable only to claWriteTextConfig
	**/

#define CLA_FLAG_IGNORE_NONSAVABLE 0x2000000
	/** Do not report about active option without CLA_CFG flag
		Applicable only to claWriteTextConfig
	**/

#define CLA_FLAG_STOP_NONSAVABLE 0x4000000
	/** Stop saving if found active option withou CLA_CFG flag
		Applicable only to claWriteTextConfig
	**/


#define CLA_FLAG_OPT_COMMENT 0x8000000
	/** Add a comment line under saved option with description 
		(generating from option list)
		Applicable only to claWriteTextConfig
	**/

#define CLA_FLAG_SAVE_NOTFOUND 0x10000000
	/** Save non active options as commented line
		Applicable only to claWriteTextConfig
	**/

#define CLA_FLAG_HEAD_INFO 0x20000000
	/** Create a simple file header with description of
		synthax of the file (CLA_MSG_TEXT_CONFIG_HEAD used)
		Applicable only to claWriteTextConfig
	**/


#define CLA_FLAG_NO_FLAGS 0x0
	/** used only for mnemonic purposes, instead 0*/


/** Debug text **/

#define CLA_MSG_DBG_BODY "--%s (type=0x%X,%d), value="
#define CLA_MSG_DBG_FREEOPT_HEAD "Free Opt List (%d)\n"



/** include keyword **/
#define CLA_KEYWORD_INCLUDE "include"


#endif

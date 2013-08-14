/** 

	* Section 1 *


**/



/*integer data type for configuration items, int for int32, long long for int64*/
#define CLA_INT long long

/*fuction to convert string to integer, atoi for int32, atoll for int64 */
#define CLA_INTC atoll



/** 
    This structure represent a option name, description and type. 
    Array of thouse structures requed by mostly all functions of library
**/
typedef struct {
	int		ID;			/** Number of option,must be unique for application
							Strongly recommended to set ID equal to order 
							number of option for enabling fast mode	**/
	char*	optName;	/** Name of option in text config and --long options
							in command line **/
	char	optLetter;  /** Short name of option (used only in command line)*/
	int		optType;	/** Type of the option (see option types) **/
	int		minAllowed; /** Minimal allowed number of arguments, 
							(0 means no requied arguments)*/
	int 	maxAllowed; /** Maximal allowed number of argumbents
							(zero means no limitation for number of arguments)
						**/
	char* 	description;/** Text description of option, used in --help,
							in WriteTextConfig for user info only*/
}claOptName_t;



/**	This structure describe a value(s) of found option. **/
typedef struct{
	int 	found;		/** Flag, displaying, that option has been found **/
	int 	ID;			/** ID (copying from claOptName_t)	**/
	int 	type;		/** Type, (see option types) **/
	int 	argNum; 	/** number of found arguments for option  **/
	union{				/** usage of nlist or slist depends on type **/
		CLA_INT* nlist;	/** array of numeric values of arguments **/
		char**	 slist;	/** array of string values of argumnets **/
	};
}claCfgItem_t;


/** 
	This structure contain a list (array) of found options and additional
	data for readed config/command line. Common used for mostly all function
	(as input or output value)
**/
typedef struct{
	int 			entryNum;	/** number of enties (must be equal to number
									of option in array of claOptName_t) **/
	claCfgItem_t 	*item;		/** pointer to array of values of arguments */
	int FreeOptNum;				/** number of 'FreeOptions' (only for command
									line **/
	char** FreeOpt;				/** Pointer to array of Free Options, number
									of entries equal to FreeOptNum **/
}claCfg_t;



/** 

	* Section 2 *
	
	Data types

	Note: all values in this section are bitflags 
	for fields optType in claOptName_t and type in claCfgItem_t.

**/



enum { /*basic data types*/
	CLA_TYPE_NUMBER=1,	/** number (signed int 32) 			*/
	CLA_TYPE_SWITCH=2,	/** switch - boolean flag (0 or 1)	*/
	CLA_TYPE_STRING=4	/** string							*/
};

enum{/* area of using option*/
	CLA_CMD=0x100,	/** command line **/
	CLA_CFG=0x200,	/** config file **/
	CLA_ENV=0x400	/** environment variable **/
};


#define CLA_GROUP_0 0x00010000
#define CLA_GROUP_1 0x00020000
#define CLA_GROUP_2 0x00040000
#define CLA_GROUP_3 0x00080000
#define CLA_GROUP_4 0x00100000
#define CLA_GROUP_5 0x00200000
#define CLA_GROUP_6 0x00400000
#define CLA_GROUP_7 0x00800000
#define CLA_GROUP_8 0x01000000
#define CLA_GROUP_9 0x02000000
#define CLA_GROUP_A 0x04000000
#define CLA_GROUP_B 0x08000000
#define CLA_GROUP_C 0x10000000
#define CLA_GROUP_D 0x20000000
#define CLA_GROUP_E 0x40000000

#define CLA_GROUP_ALL (CLA_GROUP_0|CLA_GROUP_1|CLA_GROUP_2|CLA_GROUP_3|CLA_GROUP_4|CLA_GROUP_5|CLA_GROUP_6|CLA_GROUP_7|CLA_GROUP_8|CLA_GROUP_9|CLA_GROUP_A|CLA_GROUP_B|CLA_GROUP_C|CLA_GROUP_D|CLA_GROUP_E)

#define CLA_ALL (CLA_CMD|CLA_CFG|CLA_ENV)
/** all possible areas of using **/


#define CLA_CASE_INSENSITIVE 0x20
/*ignore case for option name*/

#define CLA_HELP (CLA_TYPE_SWITCH|CLA_CMD|CLA_EXCLUSIVE)
/** default set of option for --help and --ver options **/

#define CLA_NUMBER (CLA_TYPE_NUMBER|CLA_ALL)
/**standart for number, accepting from any source **/

#define CLA_SWITCH (CLA_TYPE_SWITCH|CLA_ALL)
/**standart for switch, accepting from any source **/

#define CLA_STRING (CLA_TYPE_STRING|CLA_ALL)
/**standart for string, accepting from any source **/

#define CLA_EXCLUSIVE 0x10
/** 
	Exclusive flag (actual only for command line parsing,
	stop checking for minimal number of FreeOptions in 
    claParseCommandLine. (used for --help, --version, etc.
**/




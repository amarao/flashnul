#ifndef _H_CLA_I18N_H
#define _H_CLA_I18N_H

#define CLA_MSG_BAD_DECLARATION "Declaration is invalid (option %s's ID != order number in declaration)\n"

#define CLA_MSG_CMD_INTERNAL_ERROR_1 "internal error (claParseCommandLine): ID<0\n"
#define CLA_MSG_CMD_TOO_MUCH_SL "option --%s (-%c) - too many arguments\n"
#define CLA_MSG_CMD_INCORRECT_ARG_SL "option --%s (-%c): argument \"%s\" is not valid\n"
#define CLA_MSG_CMD_CONFLICT_L "option --%s should not be used together with --%s\n"
#define CLA_MSG_CMD_UNKNOWN_L "option --%s is not recognized\n"
#define CLA_MSG_CMD_TOO_MUCH_L "option --%s - too many arguments\n"
#define CLA_MSG_CMD_NO_ARGS_L "option --%s requed argument\n"
#define CLA_MSG_CMD_INCORRECT_ARGS_L "option --%s - incorrect argument type\n"
#define CLA_MSG_CMD_UNKNOWN_S "option -%c is not recognized\n"
#define CLA_MSG_CMD_CONFLICT_S "option -%c should not used together with -%c\n"
#define CLA_MSG_CMD_TOO_MUCH_S "option -%c - too many arguments\n"
#define CLA_MSG_CMD_ARG_NOT_FOUND_S "option -%c requed argument\n"
#define CLA_MSG_CMD_INCORRECT_S "option -%c - incorrect argument type\n"
#define CLA_MSG_CMD_ARGS_REQUED_S "option -%c requed argument\n"
#define CLA_MSG_CMD_NOT_ENOUGH_FREE "not enough arguments\n"

#define CLA_MSG_OPTION_CONFLICT "%s should not be used together with %s\n"
#define CLA_MSG_OPTION_CONFLICT_SHORT "%c should not be used together with  %c\n"

#define CLA_MSG_NO_FILENAME "No filename\n"
#define CLA_MSG_IO_ERROR "%s: %s\n"
#define CLA_MSG_SELF_RECURSION "File %s, line %d, including himself (ignoring)\n"
#define CLA_MSG_FILE_UNKNOWN_OPTION "File %s, line %d: %s - unrecognized\n"
#define CLA_MSG_FILE_NO_ENOUGH_ARGS "File %s, line %d: %s - no enough arguments\n"
#define CLA_MSG_FILE_TOO_MUCH_ARGS "File %s, line %d: %s - too many arguments\n"
#define CLA_MSG_FILE_OPTION_REQUED "File %s, line %d: %s - requed argument missed\n"
#define CLA_MSG_FILE_INCORRECT_ARG "File %s, line %d: %s - incorrect type of argument\n"
#define CLA_MSG_FILE_CONFLICT	"File %s, line %d: %s - conflict with %s (should not be used together)\n"

#define CLA_MSG_ENV_NO_ENOUGH_ARGS	"Environment line %s has no enough arguments\n"
#define CLA_MSG_ENV_TOO_MUCH_ARGS	"Environment line %s has too much arguments\n"
#define CLA_MSG_ENV_OPTION_REQUED	"Environment line %s should has argument\n"
#define CLA_MSG_ENV_INCORRECT_ARG	"Environment line %s has wrong argument type\n"
#define CLA_MSG_ENV_CONFLICT "Environment line %s should not be used together with %s\n"


#define CLA_MSG_INCORRECT_CALL "Incorrect library call (internal application error)\n"

#define CLA_MSG_NONSAVABLE "Option '%s' not saved\n"

#define CLA_MSG_TEXT_CONFIG_HEAD "#   configuration file\n#   character '#' at line begin comment line\n#   sytax:\n#   name = value\n#   name = value1;value2;...;valueNN\n\n"


#define CLA_MSG_ENV_NOT_ENOUGH "Environment line %s has no enough arguments\n"

#endif

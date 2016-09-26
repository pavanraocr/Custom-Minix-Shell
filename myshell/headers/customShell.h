/*
 * customShell.h
 *
 *  Created on: Sep 20, 2016
 *      Author: pavan
 */

#ifndef HEADERS_CUSTOMSHELL_H_
#define HEADERS_CUSTOMSHELL_H_

typedef struct aliasArray{
	char *key;
	char *value;
}aliasArray;

typedef enum statusType{EXIT = 0, ERR = 1, COMPLETED=2, PRNTERR=3}statusType;

typedef enum bool{TRUE = 1, FALSE = 0}bool;

#ifdef MAIN_FILE
	char *SHELL_HOME;

	char *PATH_VAR[20];
	int NUM_OF_PATHS;

	char **SYS_CMD_LIST;
	int NUM_OF_CMDS;

	aliasArray *ALIAS_LIST;
	int NUM_OF_ALIASES, CAPACITY_OF_ALIASES;
#else
	extern char *SHELL_HOME;

	extern char *PATH_VAR[20];
	extern int NUM_OF_PATHS;

	extern char **SYS_CMD_LIST;
	extern int NUM_OF_CMDS;

	extern aliasArray *ALIAS_LIST;
	extern int NUM_OF_ALIASES, CAPACITY_OF_ALIASES;
#endif


#endif /* HEADERS_CUSTOMSHELL_H_ */

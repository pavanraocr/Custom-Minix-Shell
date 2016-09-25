/*
 * customShell.h
 *
 *  Created on: Sep 20, 2016
 *      Author: pavan
 */

#ifndef HEADERS_CUSTOMSHELL_H_
#define HEADERS_CUSTOMSHELL_H_

#ifdef MAIN_FILE
	char *SHELL_HOME;
	char *PATH_VAR[20];
	int NUM_OF_PATHS;
#else
	extern char *SHELL_HOME;
	extern char *PATH_VAR[20];
	extern int NUM_OF_PATHS;
#endif

typedef enum statusType{EXIT = 0, ERR = 1, COMPLETED=2}statusType;


#endif /* HEADERS_CUSTOMSHELL_H_ */

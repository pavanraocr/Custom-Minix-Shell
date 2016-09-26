/*
 * alias.c
 *
 *  Created on: Sep 25, 2016
 *      Author: pavan
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//custom header definition
#include "../headers/customShell.h"

/**
 * This function prepares the ls command for a particular path given in the path variable
 *
 * returns void
 */
void prepareLSString(char *lSString, int i){
	strcpy(lSString, "/bin/ls ");
	strcat(lSString, PATH_VAR[i]);
}

/**
 * removes the new line character
 *
 * return void
 */
void removeNewLineChar(char *str){
	if(str[strlen(str)-1] == '\n'){
		str[strlen(str)-1] = '\0';
	}
}

/**
 * This initializes all the system commands to the global array
 *
 * returns statusType
 */
statusType initializeSysCommands(){

	FILE *fp;
	char path[2048];
	char *lSString;
	int i;
	int capacity = 3000; //defaultValue which will grow in logarithmic scale as required
	int numOfCommands = 0;
	char **biggersize;

	SYS_CMD_LIST = malloc(sizeof(char *)*capacity);

	for(i = 0; i < NUM_OF_PATHS; i++){

		lSString = (char *)malloc(sizeof(char)*(strlen("/bin/ls ")+strlen(PATH_VAR[i]+1)));

		prepareLSString(lSString, i);

		/*executing the command and opening the descriptor for reading the output*/
		fp = popen(lSString, "r");
		free(lSString);

		if(fp == NULL){
			printf("ERROR: Failed to execute a command\n");
			return ERR;
		}

		/*Once the command is executed successfully the output is now read*/
		while(fgets(path, sizeof(path)-1, fp) != NULL){
			if(numOfCommands == capacity){
				capacity += capacity;
				biggersize = realloc(SYS_CMD_LIST, capacity);

				if(biggersize == NULL){
					printf("ERROR: Problem in expanding the SYS_CMD_LIST\n");
					pclose(fp);
					return ERR;
				}

				SYS_CMD_LIST = biggersize;
			}

			removeNewLineChar(path);

			if(strlen(path) <= 0){
				continue;
			}

			SYS_CMD_LIST[numOfCommands] = (char *)malloc(sizeof(char)*(strlen(path)+1));

			//check if the memory was allocated
			if(SYS_CMD_LIST[numOfCommands] == NULL){
				printf("ERROR: Problem allocating memory for a particular string\n");
				pclose(fp);
				return ERR;
			}
			strcpy(SYS_CMD_LIST[numOfCommands++], path);
			//printf("=>%s\n", SYS_CMD_LIST[numOfCommands-1]);
		}

		/*closing the descriptor*/
		pclose(fp);
	}

	NUM_OF_CMDS = numOfCommands;

	return COMPLETED;
}

/**
 * checks whether the key entered is one of the system commands
 *
 * return statusTys
 */
bool checkIfSysCommand(char *key){
	int i;

	//linear search through the given system commands to see the key is a system command
	for(i = 0; i < NUM_OF_CMDS; i++){
		//printf("=%s\n",SYS_CMD_LIST[i]);
		if(strcmp(key,SYS_CMD_LIST[i]) == 0)
			return TRUE;
	}

	return FALSE;
}

/**
 * checks if the key and value are equal and return error if it is so
 * checks if the given key value pair is switched then it should not be allowed
 *
 * return statusType
 */
statusType checkIfAlreadyUsed(char *key, char *val){

	//check if both key and value are the same
	if(strcmp(key,val) == 0){
		printf("ERROR: Key and the value of the alias command are the same\n");
		return PRNTERR;
	}

	int i;

	for(i = 0; i < NUM_OF_ALIASES; i++){
		//checks if an interchanged key value pair is trying to be stored
		if(strcmp(val, ALIAS_LIST[i].key) == 0 && strcmp(key, ALIAS_LIST[i].value) == 0){
			printf("ERROR: The alias to the command %s is already an alias to the command %s\n", val, key);
			return PRNTERR;
		}

		//checks if alias already exist
		if(strcmp(key, ALIAS_LIST[i].key) == 0){
			//checks if the same command is being assigned
			if(strcmp(val, ALIAS_LIST[i].value) == 0){
				printf("ERROR: The alias mapping to the command %s already exists\n", val);
				return PRNTERR;
			}

			//if the alias is already assigned to other commands then it should not be used again
			printf("ERROR: The is alias is already a assigned for the command %s\n", ALIAS_LIST[i].value);
			return PRNTERR;
		}
	}

	return COMPLETED;
}

/**
 * This stores the value in the aliasList
 *
 * returns statusType
 */
statusType recordMapping(char *key, char *val){
	aliasArray *biggerStorage;

	//checks if the alias list is initialized at least once
	if(ALIAS_LIST == NULL){
		CAPACITY_OF_ALIASES = 100;
		ALIAS_LIST = (aliasArray *)malloc(sizeof(aliasArray *)*CAPACITY_OF_ALIASES);
		NUM_OF_ALIASES = 0;
	}

	//Checks if the capacity has reached
	if(NUM_OF_ALIASES == CAPACITY_OF_ALIASES){
		CAPACITY_OF_ALIASES += CAPACITY_OF_ALIASES;
		biggerStorage = (aliasArray *)realloc(ALIAS_LIST, CAPACITY_OF_ALIASES);

		if(biggerStorage == NULL){
			printf("ERROR: out of memory to expand CAPACITY_OF_ALIAS\n");
			return ERR;
		}

		ALIAS_LIST = biggerStorage;
	}

	//stores the key value pair
	ALIAS_LIST[NUM_OF_ALIASES].key = (char *)malloc(sizeof(char)*strlen(key));
	ALIAS_LIST[NUM_OF_ALIASES].value = (char *)malloc(sizeof(char)*strlen(val));

	strcpy(ALIAS_LIST[NUM_OF_ALIASES].key, key);
	strcpy(ALIAS_LIST[NUM_OF_ALIASES].value, val);

	NUM_OF_ALIASES++;

	return COMPLETED;
}

/**
 * This is the function that is called to store the alias command
 *
 * param commandStr - which is the command entered by the user without the keyword "ALIAS"
 * returns statusType
 */
statusType mapNewAlias(char *commandStr){

	statusType retStatus;
	bool isSysCommand;
	char delim = '=';
	char *cmdStrCopy = (char *)malloc(sizeof(char)*strlen(commandStr));

	strcpy(cmdStrCopy, commandStr);

	char *tokenVal = NULL, *key;

	tokenVal = strtok(cmdStrCopy, &delim);

	if(tokenVal == NULL){
		printf("ERROR: The alias given was incorrect\n");
		free(cmdStrCopy);
		return PRNTERR;
	}

	key = (char *)malloc(sizeof(char)*strlen(tokenVal));
	strcpy(key,tokenVal);

	tokenVal = strtok(NULL, &delim);

	if(tokenVal == NULL){
		printf("ERROR: The alias given was incorrect\n");
		free(key);
		free(cmdStrCopy);
		return PRNTERR;
	}

	isSysCommand = checkIfSysCommand(key);

	if(isSysCommand == TRUE){
		printf("ERROR: The alias can't be a system command\nUse \"man %s\" to know more about the system command\n", key);
		free(key);
		free(cmdStrCopy);
		return PRNTERR;
	}

	retStatus = checkIfAlreadyUsed(key, tokenVal);

	if(retStatus == PRNTERR){
		free(key);
		free(cmdStrCopy);
		return PRNTERR;
	}

	retStatus = recordMapping(key, tokenVal);

	if(retStatus == ERR){
		free(key);
		free(cmdStrCopy);
		return ERR;
	}

	free(cmdStrCopy);
	free(key);
	return COMPLETED;
}

/**
 * finds and replaces the alias value in the key variable
 */
bool findAliasValue(char *key){
	int i;

	for(i = 0; i < NUM_OF_ALIASES; i++){
		if(strcmp(ALIAS_LIST[i].key, key) == 0){
			strcpy(key, ALIAS_LIST[i].value);
			return TRUE;
		}
	}

	return FALSE;
}

/**
 * checks replace the command with the respective sys call or if there is no system call associated with it then prints the error
 *
 * return statusType
 */
statusType replaceCommand(char *key){
	bool isSysCall, isAliasPresent;

	isAliasPresent = findAliasValue(key);

	if(isAliasPresent == FALSE){
		printf("ERROR: Alias doesn't map to any system call\n");
		return PRNTERR;
	}

	isSysCall = checkIfSysCommand(key);

	if(isSysCall == TRUE){
		return COMPLETED;
	}
	else{
		return replaceCommand(key);
	}
}

/**
 * This returns the command string with any aliases replaced with its original system call associated with the
 *
 * Assumption - the commandStr ends with a '\0'
 * returns char * - if successful
 * returns NULL otherwise
 */
char* validateCommand(char *commandStr){
	char *key=NULL, *tokenVal=NULL, *finalCmd;
	bool isCommanArgsPresent = TRUE;
	int valueLen;
	bool isSysCmd;
	statusType retStat;
	char *cmdStrCopy = (char *)malloc(sizeof(char)*strlen(commandStr));
	strcpy(cmdStrCopy, commandStr);

	tokenVal = strtok(cmdStrCopy, " ");

	if(tokenVal == NULL){
		printf("ERROR: Command string empty\n");
		return NULL;
	}

	key = (char *)malloc(sizeof(char)*strlen(tokenVal));
	strcpy(key,tokenVal);

	isSysCmd = checkIfSysCommand(key);

	//if the command is already a system command then no need replacement
	if(isSysCmd == TRUE){
		free(key);
		return *commandStr;
	}

	tokenVal = strtok(NULL, "\0");

	//checks if there are any more arguments to append
	if(tokenVal == NULL || strlen(tokenVal) == 0){
		isCommanArgsPresent = FALSE;
		valueLen = 0;
	}
	else{
		valueLen = strlen(tokenVal);
	}

	retStat = replaceCommand(key);

	if(retStat == PRNTERR){
		//printf("ERROR: The alias %s is not registered to any command\n", key);

		free(key);
		return NULL;
	}

	finalCmd = (char *)malloc(sizeof(char)*(strlen(key)+valueLen+1));
	strcpy(finalCmd, key);

	if(isCommanArgsPresent == TRUE){
		strcat(finalCmd, " ");
		strcat(finalCmd, tokenVal);
	}

	free(key);
	return finalCmd;
}

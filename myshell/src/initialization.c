/*
 * initialization.c
 *
 *  Created on: Sep 24, 2016
 *      Author: pavan
 */

//custom header definition
#include "../headers/customShell.h"

#include<stdio.h>
#include<regex.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>


regex_t regex;
int compileStat;
char *errMsg;

/**
 * Checks the token value for HOME variable if present loads the HOME variable
 *
 * Returns statusType
 */
statusType loadVariables(char *key, char *val){
	//if there is a key then there should exist a value
	if(val == NULL){
		return ERR;
	}

	char *tokenVal;
	int i = 0;

	if(strcmp(key, "HOME") == 0){
		if(val[strlen(val)-1] == '\n'){
			val[strlen(val)-1] = '\0';
		}

		//Setting the Home Variable
		strcpy(SHELL_HOME,val);
	}else if(strcmp(key, "PATH") == 0){
		//setting the path variable
		tokenVal = strtok(val,":");

		//this is to check if the value contains only a single path
		if(tokenVal == NULL && strlen(val) > 0){
			PATH_VAR[i] = (char *)malloc(sizeof(char)*strlen(val));
			strcpy(PATH_VAR[i++], val);
			return EXIT;
		}

		//store all the : separated PATH env variables into an array
		while(tokenVal != NULL){
			if(PATH_VAR[i] == NULL)
				PATH_VAR[i] = (char *)malloc(sizeof(char)*strlen(tokenVal));
			else{
				if(sizeof(PATH_VAR[i]) < sizeof(tokenVal)){
					free(PATH_VAR[i]);
					PATH_VAR[i] = (char *)malloc(sizeof(char)*strlen(tokenVal));
				}
			}

			strcpy(PATH_VAR[i++], tokenVal);
			tokenVal = strtok(NULL,":");

			//Just to make sure the last charater in the string is a NULL character and not a new line character
			if(PATH_VAR[i-1][strlen(PATH_VAR[i-1])-1] == '\n'){
				PATH_VAR[i-1][strlen(PATH_VAR[i-1])-1] = '\0';
			}
		}

		//This is to set the variable which tells the number of paths that exist in the PATH_DIR variable
		NUM_OF_PATHS = i;
	}else{
		printf("The environment variable doesn't exist\n");
		return ERR;
	}

	return EXIT;
}

/**
 * Changes the directory to any given path as an input
 *
 * if input is NULL then value of SHELL_HOME is accessed
 * returns statusType
 */
statusType changeDir(char *dir){
	char *dirStr;
	int retStatus;

	//checks the case when the argument passed is NULL then value of SHELL_HOME is taken
	if(dir == NULL){
		dirStr = (char *)malloc(sizeof(char)*(strlen(SHELL_HOME)));
		strcpy(dirStr, SHELL_HOME);
	}
	else{
		dirStr = (char *)malloc(sizeof(char) * (strlen(dir)));
		strcpy(dirStr, dir);
	}

	retStatus = chdir(dirStr);

	if(retStatus != 0){
		printf("ERROR: Change directory operation failed\n");
		return ERR;
	}

	return EXIT;
}

/**
 * Reads the profile file that is assumed to be present in the root directory and sets certain variables
 * whose values are got from the profile file
 *
 * Returns statusType
 */
statusType loadProfile(){
	FILE *fp;
	statusType retStat=EXIT;

	//Opening the profile file
	fp = fopen(".profile.txt","r");

	if(fp == NULL){
		printf("The file doesn't exist or not readable\n");
		return ERR;
	}

	char *line = NULL;
	size_t lineLength = 0;
	char delim = '=';
	char *tokenVal = NULL;
	char *key;

	while(getline(&line, &lineLength, fp) != -1){
		printf("Tokenizing the line %s\n", line);

		//gets the env variable in the line
		tokenVal = strtok(line, &delim);

		//This is the line that doesn't contain any environment variable initialization
		if(tokenVal == NULL){
			continue;
		}

		key = (char *)malloc(sizeof(char)*strlen(tokenVal));
		strcpy(key, tokenVal);

		//gets the value of the env variable that was given as a key earlier
		tokenVal = strtok(NULL, &delim);

		//The value for a given key is missing
		if(tokenVal == NULL){
			free(key);
			continue;
		}

		loadVariables(key, tokenVal);
		free(key);
	}

	//closing the profile file
	fclose(fp);

	//changes the current directory
	retStat = changeDir(SHELL_HOME);

	return retStat;
}

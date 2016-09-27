/*
 * main.c
 *
 *  Created on: Sep 20, 2016
 *      Author: vivekbajpai
 */
#define MAIN_FILE
/* Custom header file includes */
#include"../headers/customShell.h"

#include<stdio.h>
#include<stdlib.h>
#include<regex.h>
#include<string.h>

//prototypes
void invokeShell();
statusType loadProfile();
statusType storeAliasStructure();

statusType initEnvVars(){
	//SHELL_HOME variable
	SHELL_HOME = (char *)malloc(sizeof(char)*100);
	strcpy(SHELL_HOME,"/home");

	//PATHVAR
	PATH_VAR[0] = (char *)malloc(sizeof(char)*100);
	strcpy(PATH_VAR[0],".");

	//number of paths defined
	NUM_OF_PATHS = 1;

	return EXIT;
}

statusType freeEnvVars(){
	int i;

	free(SHELL_HOME);
	for(i = 0; i < NUM_OF_PATHS; i++){
		free(PATH_VAR[i]);
	}

	return EXIT;
}

/**
 * Main function that start the loop of the terminal
 */
int main(void){

	statusType status;

	printf("Welcome to our custom Shell\n");

	invokeShell();

	printf("\nThank you have a good day!!!\n");
	return 0;
}

/**
 * Print the promt
 *
 */
void printPrompt(){
	printf("\n%s>", SHELL_HOME);
}

/**
 * Prepares the name of the alias file
 */
void prepareAFName(char *afName){
	char delim[2] = "/";

	strcpy(afName, SHELL_HOME);

	if(afName[strlen(afName)-2] != '/'){
		strcat(afName, delim);
	}

	strcat(afName, aliasMappingFile);
}

/**
 * Stores the alias mapping
 *
 * returns statusType
 */
statusType storeAliasStructure(){
	FILE *af;
	char *afName;
	int i;

	afName = (char *)malloc(sizeof(char)*(strlen(SHELL_HOME) + strlen(aliasMappingFile) + 2));

	prepareAFName(afName);

	af = fopen(afName,"w");
	free(afName);

	if(af == NULL){
		printf("ERROR: failed to create the file %s\n", afName);
		return ERR;
	}

	fprintf(af, "%d\n", NUM_OF_ALIASES);
	for(i = 0; i < NUM_OF_ALIASES; i++){
		fprintf(af, "%s\n", ALIAS_LIST[i].key);
		fprintf(af, "%s\n", ALIAS_LIST[i].value);
	}

	fclose(af);
	return COMPLETED;
}

/**
 * Read the alias mapping
 *
 * returns statusType
 */
statusType readAliasStructure(){
	FILE *af;
	char *afName;
	char str[2048];
	int i;

	afName = (char *)malloc(sizeof(char)*(strlen(SHELL_HOME) + strlen(aliasMappingFile) + 2));
	prepareAFName(afName);

	af = fopen(afName,"r");
	free(afName);

	if(af == NULL){
		return FNF;
	}

	fscanf(af,"%d",&NUM_OF_ALIASES);
	ALIAS_LIST = (aliasArray *)malloc(sizeof(aliasArray)*(NUM_OF_ALIASES+100));

	for(i = 0; i < NUM_OF_ALIASES; i++){
		fscanf(af,"%s",str);
		removeNewLineChar(str);
		ALIAS_LIST[i].key = (char *)malloc(sizeof(char) * strlen(str));
		strcpy(ALIAS_LIST[i].key,str);

		fscanf(af,"%s",str);
		removeNewLineChar(str);
		ALIAS_LIST[i].value = (char *)malloc(sizeof(char) * strlen(str));
		strcpy(ALIAS_LIST[i].value,str);
	}

	return COMPLETED;
}

/**
 * This function invokes the custom shell which basically loops till the user decides to exit
 */
void invokeShell(){
	char *cmdLine = NULL;
	int lineLength = 0;

	statusType status;

	//initializes all the environment variables to its default values
	initEnvVars();

	//Load VARs from the profile file
	status = loadProfile();

	if(status == ERR){
		printf("There was some problem in loading/parsing the profile file\n");
		freeEnvVars();
		return;
	}

	//reads the alias file if it exists
	status = readAliasStructure();

	//initializes all the system available commands that are defined in the paths given in the PATH_VARS variable
	status = initializeSysCommands();

	if(status == ERR){
		freeEnvVars();
		return;
	}

	//Continue to listen to the user
	while(1){
		printPrompt();

		//waits for the user to enter the command
		while(getline(&cmdLine, &lineLength, stdin) == -1);

		//Now the usr has entered a command
		//printf("The Entered Command is %s\n", cmdLine);

		removeNewLineChar(cmdLine);

		if(strlen(cmdLine) <= 0){
			continue;
		}

		//parses the command and creates the dataStructure
		struct Command * head = parseCommand(cmdLine);
		struct Command * temp = head;

		while(temp!=NULL){
			regex_t regex;
			int retStat;

			if(strcmp(temp->cmdStr, "exit") == 0){

				status = storeAliasStructure();

				if(status == ERR){
					printf("ERROR: There was a problem in storing a the alias mapping\n");
				}

				freeEnvVars();
				return;
			}

			//compiles the regex to check for alias keyword
			retStat = regcomp(&regex, "alias*", 0);

			if(retStat){
				printf("ERROR: Couldn't compile the regex\n");
				freeEnvVars();
				return;
			}

			//checks if the command is an alias initialization
			retStat = regexec(&regex, temp->cmdStr, 0, NULL, 0);

			//it is a match and the command is alias initialization
			if(!retStat){
				char *tempTok;
				char *tempcmd = (char *)malloc(sizeof(char) * strlen(temp->cmdStr));

				strcpy(tempcmd, temp->cmdStr);
				tempTok = strtok(tempcmd," ");
				tempTok = strtok(NULL, " ");

				if(tempTok == NULL){
					printf("ERROR: alias command used incorrectly\nUsage: alias <alias-name>=<command>\n");
				}
				else
					mapNewAlias(tempTok);

				temp = temp->nextCommand;
				free(tempcmd);
				continue;
			}
			else{
				temp->cmdStr = validateCommand(temp->cmdStr);
			}

			char * arguments[] = {temp->cmdStr,NULL};

			pid_t pid = fork();
			if(pid < 0){
				perror("fork failed!!!!");
				freeEnvVars();
				exit(-1);
			}
			else if(pid == 0){
				if(temp->siblingCommand!=NULL){
					executeInParallel(temp->siblingCommand);
				}
				execvp(arguments[0],arguments);
				exit(0);
			}
			else if(pid > 0){
				//wait for the current child to finish, hence ensuring serial execution
				wait(NULL);
				temp = temp->nextCommand;
			}
		}
	}

	freeEnvVars();
}


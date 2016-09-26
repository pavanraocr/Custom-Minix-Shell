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
#include<string.h>

//prototypes
void invokeShell();
statusType loadProfile();

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

	printf("Welcome to our custom Shell\n");

	invokeShell();

	printf("\nThank you have a good day!!!\n");
	return 0;
}

/**
 * This function invokes the custom shell which basically loops till the user decides to exit
 */
void invokeShell(){

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

	//initializes all the system available commands that are defined in the paths given in the PATH_VARS variable
	initializeSysCommands();

	//Continue to listen to the user
	while(1){
		printf("\n%s>", SHELL_HOME);

		//TESTING THE ALIAS functionality to be removed later
		mapNewAlias("listDirectory=ls");
		mapNewAlias("changeDir=cd");
		validateCommand("listDirectory");
		validateCommand("which /etc/");

		//TO-DO

		break;
	}

	freeEnvVars();
}

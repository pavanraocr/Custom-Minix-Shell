/*
 * main.c
 *
 *  Created on: Sep 20, 2016
 *      Author: vivekbajpai
 */

/* Custom header file includes */
#include"../headers/customShell.h"

#include<stdio.h>

//prototypes
void invokeShell();
statusType loadProfile();

/**
 * Main function that start the loop of the terminal
 */
int main(void){

	printf("Welcome to our custom Shell\n");
	char *SHELL_HOME = "/home/";

	invokeShell();

	printf("Thank you have a good day!!!\n");
	return 0;
}

/**
 * This function invokes the custom shell which basically loops till the user decides to exit
 */
void invokeShell(){

	statusType status;

	//Load VARs from the profile file
	status = loadProfile();

	if(status == ERR){
		printf("There was some problem in loading/parsing the profile file");
		return;
	}

	while(1){
		//TO-DO
		return;
	}
}

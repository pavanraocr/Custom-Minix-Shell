/*
 * initialization.c
 *
 *  Created on: Sep 24, 2016
 *      Author: pavan
 */

#include<stdio.h>
#include<regex.h>

//custom header definition
#include "customShell.h"

regex_t regex;
int compileStat;
char *errMsg;

/**
 * Checks the token value for HOME variable if present loads the HOME variable
 *
 * Returns statusType
 */
statusType loadVariables(char *tokenVal){
	compileStat = regexc(&regex, tokenVal, 0, NULL, 0);
	if(!compileStat){
		//This line contains the home variable definition

		return COMPLETED;
	}
	else if(compileStat != REG_NOMATCH){
		return EXIT;
	}
	else{
		regerror(compileStat, &regex, errMsg, sizeof(errMsg));
		printf("Regex failed to match with error: %s\n", errMsg);
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

	//Opening the profile file
	fp = fopen(".profile.txt","r");

	compileStat = regcomp(&regex, "HOME*",0);

	if(compileStat){
		printf("There is some error in compiling the regularExpression");
		return ERR;
	}

	char *line = NULL;
	size_t lineLength = 0;
	char delim = " ";
	char *tokenVal;

	while(getline(&line, &lineLength, fp) != -1){
		printf("Tokenizing the line %s", line);
		tokenVal = strtok(line, delim);

		//logic to understand all the token
		while(tokenVal != NULL){
			loadVariables(tokenVal);
			tokenVal = (NULL, delim);
		}
	}

	//closing the profile file
	fclose(fp);

	return EXIT;
}

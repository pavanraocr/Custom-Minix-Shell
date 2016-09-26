/*
 * executer.c
 *
 *  Created on: Sep 24, 2016
 *      Author: vivekbajpai
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

void tokenizeFirstLevel(char* retArrray[15], int * retCount, char * str);
struct Command * parseCommand(char* cmd);
void printList(struct Command * head);
void executeInParallel(struct Command * head);
void executeInSerial(struct Command * head);


struct Command {
   char *  cmdStr;
   struct Command * nextCommand ;
   struct Command * siblingCommand ;
}*cmdHead;

void appendAtLast(struct Command *head, struct Command *element){
	if(NULL==head){
		head =  element;
		return;
	}
	struct Command * temp = head;
	while (temp->nextCommand !=NULL){
		temp = temp->nextCommand;
	}
	temp->nextCommand = element;
}

void addAsSibling(struct Command *head, struct Command *element){
	if(NULL==head){
		head =  element;
		return;
	}
	struct Command * temp = head;
	while (temp->siblingCommand !=NULL){
		temp = temp->siblingCommand;
	}
	temp->siblingCommand = element;
}

void trimParenthesis(char **str){
	char* temp = *str;
	while((*temp)!='\0'){
		temp=temp+1;
	}
	*(temp-1) = '\0';
	*str = *str+1;
}

void printSibling(struct Command * head){
	struct Command * temp = head;
	while(temp!=NULL){
		printf("%s\n",temp->cmdStr);
		temp=temp->siblingCommand;
	}
}

struct Command * getSiblingCommands(char * str){
	int cmdCount;
	int i;
	char* cmdArray[35];
	tokenizeFirstLevel(cmdArray,&cmdCount,str);
	struct Command * head = malloc(sizeof(struct Command));
	head->cmdStr = cmdArray[0];
	head->nextCommand=NULL;
	head->siblingCommand=NULL;

	for(i=1;i<cmdCount;i++){
		char * currCmd = cmdArray[i];
		struct Command * temp = malloc(sizeof(struct Command));

		if(currCmd[0]=='('){
			trimParenthesis(&currCmd);
			temp=parseCommand(currCmd);
			addAsSibling(head,temp);
		}
		else{
			temp->cmdStr = currCmd;
			temp->nextCommand=NULL;
			temp->siblingCommand=NULL;
			addAsSibling(head,temp);
		}
	}
	return head;
}

struct Command * parseCommand(char* cmd){
	struct Command* parenthesizedHead=NULL;
	struct Command* nonParenthesizedHead=NULL;
	int i;
	char* cmdArray[35];
	int cmdCount;
	tokenizeFirstLevel(cmdArray,&cmdCount,cmd);
	for(i=0;i<cmdCount;i++){
		char* currCmd = cmdArray[i];
		struct Command * temp = malloc(sizeof(struct Command));
		if(currCmd[0]=='('){
			trimParenthesis(&currCmd);
			temp =getSiblingCommands(currCmd);
			//append to the non parenthesized list
			if(NULL==parenthesizedHead){
				parenthesizedHead = temp;
			}
			else{
				appendAtLast(parenthesizedHead,temp);
			}
		}
		else{
			temp->cmdStr= currCmd;
			temp->nextCommand = NULL;
			temp->siblingCommand = NULL;

			//append to the non parenthesized list
			if(NULL==nonParenthesizedHead){
				nonParenthesizedHead = temp;
			}
			else{
			appendAtLast(nonParenthesizedHead,temp);
			}
		}
	}

	if(NULL == parenthesizedHead){
		return nonParenthesizedHead;
	}else{
		appendAtLast(parenthesizedHead,nonParenthesizedHead);
		return parenthesizedHead;
	}
}

void printList(struct Command * head){
	struct Command * temp = head;
	while(temp!=NULL){
		printf("%s\n",temp->cmdStr);
		temp=temp->nextCommand;
	}
}



int main(void){
	//char * str = "ls,(pwd,cmd3,(cmd4,cmd5)),cmd6\0";
	char* str = "/bin/ls,(/bin/pwd,/bin/ls)\0";
	struct Command * head = parseCommand(str);

	struct Command * temp = head;

	while(temp!=NULL){
		pid_t pid = fork();
		char * arguments[] = {temp->cmdStr,NULL};
		if(pid<0){
			perror("fork failed!!!!");
			exit(-1);
		}
		else if(pid==0){
			execvp(arguments[0],arguments);
			exit(0);

			/*if(temp->siblingCommand==NULL){
				execvp(arguments[0],arguments);
				exit(0);
			}else{
				//printSibling(temp);
				//executeInParallel(temp);
				exit(0);
			}*/
		}
		else if(pid>0){
			//wait for the current child to finish, hence ensuring serial execution
			wait(NULL);
			temp = temp->nextCommand;
		}
	}

}

void executeInSerial(struct Command * head){
	struct Command * temp = head;

		while(temp!=NULL){
			pid_t pid = fork();
			char * arguments[] = {temp->cmdStr,NULL};
			if(pid<0){
				perror("fork failed!!!!");
				exit(-1);
			}
			else if(pid==0){
				if(temp->siblingCommand==NULL){
					execvp(arguments[0],arguments);
				}else{
					executeInParallel(temp);
				}
			}
			else if(pid>0){
				//wait for the current child to finish, hence ensuring serial execution
				wait(NULL);
				temp = temp->nextCommand;
			}
		}

}

void executeInParallel(struct Command * head){
	struct Command * temp = head;
	int numOfChildren = 0;
	while(temp!=NULL){
		pid_t pid = fork();
		numOfChildren++;
		char * arguments[] = {temp->cmdStr,NULL};
		if(pid==0){
			printf("i am here with ");
			exit(0);
		}
		else{
			printf("I AM THERE");
			temp=temp->siblingCommand;
		}
		/*if(pid<0){
			perror("fork failed!!!!");
			exit(-1);
		}
		else if(pid==0){
			if(temp->nextCommand==NULL){
				execvp(arguments[0],arguments);
			}else{
				//executeInSerial(temp);
			}
		}
		else if(pid>0){
			temp = temp->siblingCommand;
		}*/



	//wait for all the children running in parallel to finish
	//for(;numOfChildren>0;numOfChildren--){
		//wait(NULL);
	}

}

void tokenizeFirstLevel(char* retArray[35], int * retCount, char * str){
	short parenthesisCount = 0;
	char currChar;
	char* currToken = (char *) malloc(30);
	char delim = ',';
	char openParentheises = '(';
	char closeParenthesis = ')';
	int i,j=0;
	*(retCount) = 0;
	currChar = *(str);
	for(i=0;'\0'!=currChar;i++){
		currChar = *(str+i);
		if(currChar == delim && parenthesisCount==0){
			*(currToken+j) = '\0';
			retArray[(*(retCount))++] = currToken;
			j=0;

			currToken = (char *) malloc(30);

		}
		else
		{
			if(currChar == openParentheises && parenthesisCount==0){

				parenthesisCount++;
			}
			else if(currChar == closeParenthesis && parenthesisCount>0){
				parenthesisCount--;
			}

			*(currToken +j++)=currChar;
		}
	}
	*(currToken+j) = '\0';
	retArray[(*(retCount))++]=currToken;
	return ;
}

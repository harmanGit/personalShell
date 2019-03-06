#include <stdio.h>
#include <stdlib.h> //exit
#include <sys/types.h> // pid_t
#include <sys/wait.h> //wait
#include <unistd.h> //fork, execlp, pipe
#include<string.h>
#include <ctype.h>
#include <limits.h>

#define INDICATOR "~~>"
#define SHELLNAME "someNameForShell"

void userInputLoop();
void shellIndicator();
void lowercaseUserInput(char *rawUserInput, int userInputLength);
void parseUserInput(char *rawUserInput,char **userInputArray);
void listDirectories();
void printCurrentWorkingDirectory();
void changeDirectory(char **userInputTokenArray, int maxToken);
void getCommand(char **userInputTokenArray, int maxToken);
char* commandGenerator(char **userInputTokenArray, int maxToken);
void printOutMainArray(char **userInputArray);//BUG REMOVE THIS


void shellIndicator(){
	printf ("%s %s", SHELLNAME, INDICATOR);
}

//USERINPUT
void lowercaseUserInput(char *rawUserInput, int userInputLength){
		//to lower case userinput
		int i;
		for(i = 0; i<userInputLength; i++){
			rawUserInput[i] = tolower(rawUserInput[i]);
		}
}

void parseUserInput(char *rawUserInput,char **userInputArray){
	
	
	if(strlen(rawUserInput) > 1)
	rawUserInput[strlen(rawUserInput) - 1] = '\0'; //removing newline
	

	int i = 0;
	char *temp = strtok (rawUserInput, " ");

	while (temp != NULL)
	{
		userInputArray[i++] = temp;
		temp = strtok (NULL, " ");
	}
}

//SHELL FUNCTIONS
char* commandGenerator(char **userInputTokenArray, int maxToken){
	char *result = malloc(maxToken);
	int i = 0;
	for(i = 1; userInputTokenArray[i] != NULL && 
		   strcmp(userInputTokenArray[i], "|") != 0; i++){
		if(i >1)
		 strcat(result, " ");
		
		strcat(result, userInputTokenArray[i]);
	}

	printf("%s: \n", result);

	return result;
}

void getCommand(char **userInputTokenArray, int maxToken){
	char *firstCommand = userInputTokenArray[0];

	if (strcmp(firstCommand, "exit") == 0)
	{
		printf("Goodbye!\n");
		exit(0);
	}
	else if (strcmp(firstCommand, "cd") == 0)
	{
	  	printf("CD pressed!\n");
          	changeDirectory(userInputTokenArray, maxToken);
	}else if (strcmp(firstCommand, "ls") == 0)
	{
		  listDirectories(userInputTokenArray);
	}else if (strcmp(firstCommand, "pwd") == 0)
	{
		  printCurrentWorkingDirectory();
	}else
		printf("Invalid Input\n");
}

void printCurrentWorkingDirectory(){
	char cwd[PATH_MAX];
   	if (getcwd(cwd, sizeof(cwd)) != NULL)
	       printf("%s: %s\n", INDICATOR,cwd);
}

void changeDirectory(char **userInputTokenArray, int maxToken){
	if(chdir(commandGenerator(userInputTokenArray, maxToken)) != 0)
		printf("ERROR: CD FAILED!\n");
}

void listDirectories(char **userInputTokenArray){
	char *argv[3];
		
	argv[0] = userInputTokenArray[0];
	argv[1] = userInputTokenArray[1];
	argv[2] = userInputTokenArray[2];

	if(fork() == 0) //child
		execvp(argv[0],argv);
	else
		wait(0); //signatures: pid_t wait(int* exit_status)

}

//MAIN LOOP
void userInputLoop(){
	size_t userInputLength = 128;
	int maxTokens = 12;
	char *userInputTokenArray[maxTokens];
	char *rawUserInput;
	char *exit = "exit";

	do {
	  	shellIndicator();
		//setting everything in the array eqaul to null
		memset(&userInputTokenArray[0],'\0',userInputLength);
	 	getline (&rawUserInput, &userInputLength, stdin);
		lowercaseUserInput(rawUserInput,userInputLength);
		parseUserInput(rawUserInput, userInputTokenArray);
		getCommand(userInputTokenArray, maxTokens);

	} while(1);//loop runs as long as its not exited
	free(userInputTokenArray);
	free(rawUserInput);
	free(exit);
}

void printOutMainArray(char **userInputTokenArray){//BUG REMOVE THIS
	int i;
	for(i = 0; userInputTokenArray[i] != NULL; i++)
  		printf("MAIN ARRAY: %s\n", userInputTokenArray[i]);
}

int main(int argc, char *argv[])
{
	userInputLoop();
  return 0;
}

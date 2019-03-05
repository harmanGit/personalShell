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
void listDirectory();
void changeDirectory(char **userInputTokenArray);
void getCommand(char **userInputTokenArray);
char* commandGenerator(char **userInputTokenArray);
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
char* commandGenerator(char **userInputTokenArray){
	char *result = malloc(4);
    strcpy(result, userInputTokenArray[0]);
    strcat(result, userInputTokenArray[1]);
    return result;
	//int i;
//for(i = 0; (strcmp(userInputTokenArray[i], "|")) != 0 ||
	//userInputTokenArray[i] != NULL; i++)
//	  strncat(s, s,userInputTokenArray[i]);
}

void getCommand(char **userInputTokenArray){
	char *firstCommand = userInputTokenArray[0];

	if (strcmp(firstCommand, "exit") == 0)
	{
		printf("Goodbye!\n");
		exit(0);
	}
	else if (strcmp(firstCommand, "cd") == 0)
	{
	  printf("CD pressed!\n");
		changeDirectory(userInputTokenArray);
	}else if (strcmp(firstCommand, "ls") == 0)
	{
		  listDirectory();
	}else
		printf("Invalid Input\n");
}

void listDirectory(){
	char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
	       printf("	%s: %s\n", INDICATOR,cwd);
}

void changeDirectory(char **userInputTokenArray){
	chdir("cd ../Downloads");
}

//MAIN LOOP
void userInputLoop(){
	size_t userInputLength = 128;
	int maxTokens = 4;
	char *userInputTokenArray[maxTokens];
	char *rawUserInput;
	char *exit = "exit";

	do {
	  shellIndicator();
		getline (&rawUserInput, &userInputLength, stdin);
		lowercaseUserInput(rawUserInput,userInputLength);
		parseUserInput(rawUserInput, userInputTokenArray);
		getCommand(userInputTokenArray);
		printOutMainArray(userInputTokenArray);
		free(userInputTokenArray);

	} while(1);	//loop runs as long as the first word isnt exit
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

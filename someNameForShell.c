#include <stdio.h>
#include <stdlib.h> //exit
#include <sys/types.h> // pid_t
#include <sys/wait.h> //wait
#include <unistd.h> //fork, execlp, pipe
#include<string.h>
#include <ctype.h>

#define INDICATOR "~~>"
#define SHELLNAME "someNameForShell"

void userInputLoop();
void shellIndicator();
void lowercaseUserInput(char *rawUserInput, int userInputLength);
void parseUserInput(char *rawUserInput,char **userInputArray);
void changeDirectory();
void changeDirectory(char *userInput);
void getCommand(char *userInput);
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
void getCommand(char *userInput){

	if (strcmp(userInput, "exit") == 0)
	{
		printf("Goodbye!\n");
		exit(0);
	}
	else if (strcmp(userInput, "cd") == 0)
	{
	  printf("CD pressed!\n");
		changeDirectory(userInput);
	}else
		printf("Invalid Input\n");
}

void changeDirectory(char *userInput){
	chdir(userInput);
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
		getCommand(userInputTokenArray[0]);
		printOutMainArray(userInputTokenArray);

	} while(1);	//loop runs as long as the first word isnt exit

}

void printOutMainArray(char **userInputArray){//BUG REMOVE THIS
	int i;
	for(i = 0; userInputArray[i] != NULL; i++)
   printf("MAIN ARRAY: %s\n", userInputArray[i]);
}

int main(int argc, char *argv[])
{
	userInputLoop();
  return 0;
}

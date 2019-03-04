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
void parseUserInput(char *rawUserInput,char *userInputArray[]);
void changeDirectory();
int stringCompare(char *firstString, char *secondString);


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

		//stringCompare("exit", userInputTokenArray[0]);
		if(strcmp(exit,userInputTokenArray[0]) == 0)
			printf("I FUCKING WORK :/\n" );

	} while(1);	//loop runs as long as the first word isnt exit

	printf("Goodbye!\n");
}

void shellIndicator(){
	printf ("%s %s", SHELLNAME, INDICATOR);
}

void lowercaseUserInput(char *rawUserInput, int userInputLength){
		//to lower case userinput
		int i;
		for(i = 0; i<userInputLength; i++){
			rawUserInput[i] = tolower(rawUserInput[i]);
		}
}

void parseUserInput(char *rawUserInput,char *userInputArray[]){
	rawUserInput[strlen(rawUserInput) - 1] = '\0'; //removing newline

	int i = 0;
	char *temp = strtok (rawUserInput, " ");

	while (temp != NULL)
	{
		userInputArray[i++] = temp;
		temp = strtok (NULL, " ");
	}
}

void changeDirectory(){
}


int main(int argc, char *argv[])
{
	userInputLoop();
  return 0;
}

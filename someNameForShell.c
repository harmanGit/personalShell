#include <stdio.h>
#include <stdlib.h> //exit
#include <sys/types.h> // pid_t
#include <sys/wait.h> //wait
#include <unistd.h> //fork, execlp, pipe
#include<string.h>
#include <ctype.h>

#define INDICATOR "~~>"
#define SHELLNAME "someNameForShell"

void shellIndicator();
void lowercaseUserInput(char *rawUserInput, int userInputLength);
void parseUserInput(char *rawUserInput,char **userInputArray);
void changeDirectory();


void userInputLoop(){
	size_t userInputLength = 128;
	int maxTokens = 3;
	char *userInputTokenArray[maxTokens];
	char *rawUserInput;
	char *exit = "exit";//char *exit = "exit";

	//printf ("	You entered: %s\n", rawUserInput);
	
	do {
	shellIndicator();
		getline (&rawUserInput, &userInputLength, stdin);
		lowercaseUserInput(rawUserInput,userInputLength);
		//printf ("before the parsing: %s", rawUserInput);
		parseUserInput(rawUserInput, userInputTokenArray);
		
		int i;
		for (i = 0; i < 3; ++i) 
				printf("%s\n", userInputTokenArray[i]);
	} while( strcmp(exit, userInputTokenArray[0]) != 0 );
	//loop runs as long as the first word isnt exit
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

void parseUserInput(char *rawUserInput,char **userInputArray){

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

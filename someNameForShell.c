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
void parseUserInput(char *rawUserInput,char *userInputArray);
void changeDirectory();


void userInputLoop(){
	size_t userInputLength = 128;
	int maxTokens = 32;
	char *userInputTokenArray[maxTokens];
	char *rawUserInput;
	char exit[4] = "exit";
	
	shellIndicator();
	getline (&rawUserInput, &userInputLength, stdin);
	lowercaseUserInput(rawUserInput,userInputLength);
	parseUserInput(rawUserInput, userInputTokenArray);

	//printf ("	You entered: %s\n", rawUserInput);
	
	while(strcmp(exit,rawUserInput)){
		
		shellIndicator();
		getline (&rawUserInput, &userInputLength, stdin);
		lowercaseUserInput(rawUserInput,userInputLength);
		parseUserInput(rawUserInput, userInputTokenArray);
		//printf ("	You entered: %s\n", rawUserInput);
	}
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

void parseUserInput(char *rawUserInput,char *userInputArray){
	userInputArray = strtok(rawUserInput, " ");
	while (userInputArray != NULL) {
		printf("%s\n", userInputArray);
		userInputArray= strtok(NULL, " ,");
	}
}

void changeDirectory(){
}


int main(int argc, char *argv[])
{
	userInputLoop();
   return 0;
}

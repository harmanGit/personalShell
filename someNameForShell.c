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
void lowercaseUserInput(char *userInput, int userInputLength);
void changeDirectory();

void userInputLoop(){
	size_t userInputLength = 128;
	char *userInput;
	char exit[4] = "exit";
	
	shellIndicator();
	getline (&userInput, &userInputLength, stdin);
	lowercaseUserInput(userInput,userInputLength);

	printf ("	You entered: %s\n", userInput);
	
	while(strcmp(exit,userInput)){
		
		shellIndicator();
		getline (&userInput, &userInputLength, stdin);
		lowercaseUserInput(userInput,userInputLength);
		
		printf ("	You entered: %s\n", userInput);
	}
}

void shellIndicator(){
	printf ("%s %s", SHELLNAME, INDICATOR);
}

void lowercaseUserInput(char *userInput, int userInputLength){
		//to lower case userinput
		int i;
		for(i = 0; i<userInputLength; i++){
			userInput[i] = tolower(userInput[i]);
		}
}

void changeDirectory(){
}


int main(int argc, char *argv[])
{
	userInputLoop();
	
   return 0;
}

#include <stdio.h>
#include <stdlib.h> //exit
#include <sys/types.h> // pid_t
#include <sys/wait.h> //wait
#include <unistd.h> //fork, execlp, pipe
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <fcntl.h>

#define INDICATOR "~~>"
#define SHELLNAME "someNameForShell"

void userInputLoop();
void shellIndicator();
void lowercaseUserInput(char *rawUserInput, int userInputLength);
void parseUserInput(char *rawUserInput,char **userInputArray);
void basicLinuxCommands(char **userInputTokenArray, int maxToken);
int redirect(int position, char **userInputTokenArray, char **argv);
int checkRedirect(char *command);
void redirectCommand(int commandId, int position, char **userInputTokenArray , char **argv);
void executeRedirect(int position, char **userInputTokenArray, char **argv);
int commandEndPosition(char **userInputTokenArray, int maxToken,int position);
void executeRedirectWrite(int position, char **userInputTokenArray, char **argv);
void executeInBackground(char **userInputTokenArray, char **argv);
void executePipe(int position, char **userInputTokenArray, char **argv);
void changeDirectory(char **userInputTokenArray, int maxToken);
void getCommand(char **userInputTokenArray, int maxToken);
void printOutMainArray(char **userInputArray);//BUG REMOVE THIS
void writingToFile();//BUG


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
int commandEndPosition(char **userInputTokenArray, int maxToken, int position){
	int commandEnd = 0;
	int i;
	for(i = position; userInputTokenArray[i] != NULL &&
		   checkRedirect(userInputTokenArray[i]) == 0; i++)
				 commandEnd++;

	return commandEnd;
}

void getCommand(char **userInputTokenArray, int maxToken){
	char *firstCommand = userInputTokenArray[0];

	if (strcmp(firstCommand, "exit") == 0)
	{
		printf("Goodbye!\n");
		exit(0);
	}
	else if (strcmp(firstCommand, "cd") == 0)
          	changeDirectory(userInputTokenArray, maxToken);
	else
		  basicLinuxCommands(userInputTokenArray, maxToken);

}

int checkRedirect(char *command){

	if(command == NULL){
		return 0;
	}if(strcmp(command, "&") == 0){
		return 38;
	}if(strcmp(command, ">") == 0){
		return 62;
	}if(strcmp(command, ">>") == 0){
		return 6262;
	}if(strcmp(command, "<") == 0){
		return 60;
	}if(strcmp(command, "|") == 0){
		return 124;
	}
	return 0;
}

void redirectCommand(int commandId, int position, char **userInputTokenArray , char **argv){
	switch (commandId)
	{
		case 38:
  		executeInBackground(userInputTokenArray, argv);//bug
  		break;
		case 62:
  		executeRedirect(position,userInputTokenArray, argv);
  		break;
		case 6262:
  		executeRedirectWrite(position,userInputTokenArray, argv);
  		break;
		case 60:
  		printf("Attempting a <, feature does not exist.\n");//executeRedirect(position,userInputTokenArray, argv);
  		break;
		case 124:
	  	executePipe(position,userInputTokenArray, argv);
	  	break;
		default :
			break;
	}
}

void changeDirectory(char **userInputTokenArray, int maxToken){
	char *result = malloc(maxToken);
	int i = 0;
	for(i = 1; userInputTokenArray[i] != NULL &&
		   checkRedirect(userInputTokenArray[i]) != 0; i++){
		if(i >1)
		 strcat(result, " ");
		strcat(result, userInputTokenArray[i]);
	}
	chdir(result);
}

void basicLinuxCommands(char **userInputTokenArray,int maxToken){
	char *argv[maxToken];
	int endOrRedirectPosition = commandEndPosition(userInputTokenArray, maxToken,0);
	//printf("argv %s\n", userInputTokenArray[endOrRedirectPosition]);//DEBUGING
	int commandId = checkRedirect(userInputTokenArray[endOrRedirectPosition]);
  //printf("argv %d\n", commandId);//DEBUGING

	int i;
	for(i = 0; i < maxToken; i++)
	{
		if(i < endOrRedirectPosition)
			argv[i] = userInputTokenArray[i];
		else
			argv[i] = NULL;
			//printf("argv %s\n", argv[i]);//DEBUGING
	}

	if(commandId != 0){
		redirectCommand(commandId,endOrRedirectPosition, userInputTokenArray, argv);
		return;
	}

		if(fork() == 0){ //child
			execvp(argv[0],argv);
			exit(0);
		}else
			wait(0);


	//char *argv[3];

	//argv[0] = userInputTokenArray[0];

	//if(userInputTokenArray[1] != NULL && redirect(1,userInputTokenArray,argv) == 0)
//		return;
//	else
//		argv[1] = userInputTokenArray[1];

//	if(userInputTokenArray[2] != NULL && redirect(2,userInputTokenArray,argv) == 0)
//		return;
//	else
//		argv[2] = userInputTokenArray[2];


//	if(fork() == 0){ //child
//		execvp(argv[0],argv);
//		exit(0);
//	}else
//		wait(0);



	//if(userInputTokenArray[1] != NULL && userInputTokenArray[2] == NULL){
	//	redirect(1,userInputTokenArray,argv);
	//	return;
	//}else
	//	argv[1] = userInputTokenArray[1];
	//
	//if(userInputTokenArray[2] != NULL){
	//	redirect(2,userInputTokenArray,argv);
	//	printf("argv %s.\n",argv[2] );
	//	return;
	//}else
	//	argv[2] = userInputTokenArray[2];
	//
	//if(fork() == 0){ //child
	//	execvp(argv[0],argv);
	//	exit(0);
	//}else
	//	wait(0);
}

void executeInBackground(char **userInputTokenArray, char **argv){
	printf("BUG: this is broken\n");
	if(fork() == 0){ //child
		execvp(argv[0],argv);
	}else
		wait(0);
}

void executePipe(int position, char **userInputTokenArray, char **argv){

   char *argv2[3];
	argv2[0] = userInputTokenArray[position+ 1];
	printf("");
	argv2[1] = userInputTokenArray[position+ 2];
	argv2[2] = userInputTokenArray[position+ 3];

   int pipefd[2];
   pipe(pipefd);

   if(fork() == 0){
      close(pipefd[0]);//close the read end of the pipe
      dup2(pipefd[1], 1);// connects write end of pipe to stdout
      execvp(argv[0],argv);//[cat pipe.c] writes to pipe
   } else if(fork() == 0){
      close(pipefd[1]); //close write end of pipe
      dup2(pipefd[0], 0);// connects write end of pipe to stdout
      execvp(argv2[0],argv2);//[grep ^#] reads from pipe
   }

   close(pipefd[0]);
   close(pipefd[1]);
   wait(NULL);
   wait(NULL);
}

void executeRedirectWrite(int position, char **userInputTokenArray, char **argv){
	int saved_stdout = dup(1);

	int file_id = open(userInputTokenArray[position + 1], O_APPEND | O_WRONLY, 0666);
	close(1); // close std output  stream
	dup(file_id); // duplicate file descriptor in slot 1
	if(fork() == 0){//child
		execvp(argv[0],argv);
		exit(0);
	}
	else
		wait(0);
	close(file_id);

	// Redirect output back to stdout
	fflush(stdout);
	dup2(saved_stdout, 1);
}

void executeRedirect(int position, char **userInputTokenArray, char **argv){
		int saved_stdout = dup(1);

		int file_id = open(userInputTokenArray[position + 1], O_CREAT | O_WRONLY, 0666);
		close(1); // close std output  stream
		dup(file_id); // duplicate file descriptor in slot 1
		if(fork() == 0){//child
			execvp(argv[0],argv);
			exit(0);
		}
		else
			wait(0);
		close(file_id);

		// Redirect output back to stdout
		fflush(stdout);
		dup2(saved_stdout, 1);
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
		getCommand(userInputTokenArray, 12);//12 is the maxToken BUG
		//printOutMainArray(userInputTokenArray);//For DEBUGING

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

void writingToFile(){//BUG

	if(fork() == 0){ //child
		  // duplicate the file descriptor for stdout
  		  int saved_stdout = dup(1);

		  int file_id = open("output.dat", O_CREAT | O_WRONLY, 0666);
		  close(1); // close std output  stream
		  dup(file_id); // duplicate file descriptor in slot 1
		  execlp("date","", NULL);
		  close(file_id);
	}else
		wait(0); //signatures: pid_t wait(int* exit_status)

}

int main(int argc, char *argv[])
{
	userInputLoop();
	return 0;
}

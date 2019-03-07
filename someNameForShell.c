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
void printCurrentWorkingDirectory();
void basicLinuxCommands(char **userInputTokenArray);
int redirect(int position, char **userInputTokenArray, char **argv);
void executeRedirect(int position, char **userInputTokenArray,char **argv);
void executeRedirectWrite(int position, char **userInputTokenArray, char **argv);
void executeInBackground(char **userInputTokenArray, char **argv);
void executePipe(int position, char **userInputTokenArray, char **argv);
void changeDirectory(char **userInputTokenArray, int maxToken);
void getCommand(char **userInputTokenArray, int maxToken);
char* commandGenerator(char **userInputTokenArray, int maxToken);
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
char* commandGenerator(char **userInputTokenArray, int maxToken){
	char *result = malloc(maxToken);
	int i = 0;
	for(i = 1; userInputTokenArray[i] != NULL && 
		   strcmp(userInputTokenArray[i], "|") != 0; i++){
		if(i >1)
		 strcat(result, " ");
		
		strcat(result, userInputTokenArray[i]);
	}

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
          	changeDirectory(userInputTokenArray, maxToken);
	else
		  basicLinuxCommands(userInputTokenArray);
	
}

void printCurrentWorkingDirectory(){
	char cwd[PATH_MAX];
   	if (getcwd(cwd, sizeof(cwd)) != NULL)
	       printf("%s: %s\n", INDICATOR,cwd);
}

void changeDirectory(char **userInputTokenArray, int maxToken){
	chdir(commandGenerator(userInputTokenArray, maxToken));
}

void basicLinuxCommands(char **userInputTokenArray){
	char *argv[3];
	
	argv[0] = userInputTokenArray[0];
	
	if(userInputTokenArray[1] != NULL && redirect(1,userInputTokenArray,argv) == 0)
		return;
	else
		argv[1] = userInputTokenArray[1];
	
	if(userInputTokenArray[2] != NULL && redirect(2,userInputTokenArray,argv) == 0)
		return;
	else
		argv[2] = userInputTokenArray[2];
		
	
	if(fork() == 0){ //child
		execvp(argv[0],argv);
		exit(0);
	}else
		wait(0);
		
	
	
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

int redirect(int position, char **userInputTokenArray, char **argv){
	if(strcmp(userInputTokenArray[position], "&") == 0){
		executeInBackground(userInputTokenArray, argv);//bug
		return 0;
	}if(strcmp(userInputTokenArray[position], ">") == 0){
		executeRedirect(position,userInputTokenArray, argv);
		return 0;
	}if(strcmp(userInputTokenArray[position], ">>") == 0){
		executeRedirectWrite(position,userInputTokenArray, argv);
		return 0;
	}if(strcmp(userInputTokenArray[position], "<") == 0){
		printf("Attempting a <, feature does not exist.\n");//executeRedirect(position,userInputTokenArray, argv);
		return 0;
	}if(strcmp(userInputTokenArray[position], "|") == 0){
		executePipe(position,userInputTokenArray, argv);
		return 0;
	}
	return 1;
	
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
	
	//int saved_stdout = dup(1);

	//FILE *file_id = fopen("hello.txt", "w");
	//close(1); // close std output  stream
	//dup(file_id); // duplicate file descriptor in slot 1
	//if(fork() == 0) //child
	//	execvp(argv[0],argv);
	//else
	//	wait(0);
	//fclose(file_id);

	// Redirect output back to stdout
	//fflush(stdout);
	//dup2(saved_stdout, 1);
		
}

void executeRedirect(int position, char **userInputTokenArray, char **argv){
	int saved_stdout = dup(1);

	int file_id = open(userInputTokenArray[position+1], O_CREAT | O_WRONLY, 0666);
	close(1); // close std output  stream
	dup(file_id); // duplicate file descriptor in slot 1
	if(fork() == 0) //child
		execvp(argv[0],argv);
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

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
#define SHELLNAME "woof"

void userInputLoop();
void shellIndicator();
void parseUserInput(char *rawUserInput,char **userInputArray);
void basicLinuxCommands(char **userInputTokenArray, int maxToken);
int redirect(int position, char **userInputTokenArray, char **argv);
int checkRedirect(char *command);
void redirectCommand(int commandId, int position,int maxToken,char **userInputTokenArray , char **argv);
void executeRedirect(int position, char **userInputTokenArray, char **argv);
int commandEndPosition(char **userInputTokenArray, int maxToken,int position);
void executeRedirectWrite(int position, char **userInputTokenArray, char **argv);
void executeInBackground(char **userInputTokenArray, char **argv);
void executeIndirect(int position, char **userInputTokenArray, char **argv);
void executePipe(int position,int maxToken ,char **userInputTokenArray, char **argv);
void changeDirectory(char **userInputTokenArray, int maxToken);
void getCommand(char **userInputTokenArray, int maxToken);
void printOutMainArray(char **userInputArray);//BUG REMOVE THIS
void writingToFile();//BUG


void shellIndicator(){
	printf ("%s %s", SHELLNAME, INDICATOR);
}

void displayASCIIArt(){
printf("                                        :NOX \n");
printf("                                       ,NOM@: \n");
printf("                                       :NNNN: \n");
printf("                                       :XXXON \n");
printf("                                       :XoXXX. \n");
printf("                                       MM;ONO: \n");
printf("  .oob..                              :MMO;MOM \n");
printf(" dXOXYYNNb.                          ,NNMX:MXN \n");
printf(" Mo.  ..:Nbb                        dNMMN MNN: \n");
printf(" @O :;XXMN...X@b.                  ,NXOMXM MNX: \n");
printf(" YX;;NMMMM@M;;OM@o.                dXOOMMN:MNX: \n");
printf(" .MOONM@@@MMN:.:NONb.            ,dXONM@@MbMXX: \n");
printf("  MOON@M@@MMMM;;:OOONb          ,MX..:ONMMMMX: \n");
printf("  :NOOM@@MNNN@@X;""XNN@Mb     .dP.   ,..OXM@N: \n");
printf("   MOON@@MMNXXMMO  :M@@M...@o.oN"":OOOXNNXXOo:\n");
printf("   :NOX@@@MNXXXMNo :MMMM@K`,:;NNM@@NXM@MNO;..N. \n");
printf("    NO:X@@MNXXX@@O:.X@@@@MOOOXMM@M@NXXN@M@NOO ..b \n");
printf("    `MO..NMNXXN@@N: .XXM@NMMXXMM@M@XO.""XM@X;.  :b \n");
printf("     YNO;.NXXXX@M;;::XMNN:OOON@@MO: ,;;.:Y@X: :OX. \n");
printf("      Y@Mb;;XNMM@@@NO: .:O: .OXN@@MO ONMMX:`XO; :X@. \n");
printf("      .@XMX.:OX@@MN:    ;O;  :OX@MO .OMM@N; .:OO;N@N \n");
printf("       YN;::.:OXMXX.: ,:NNO;.;XMMX:  ,;@@MNN...:O;:@X: \n");
printf("       `@N;;XOOOXO;;:O;:@MOO;:O:"" ,oMP@@KYM.;NMO;`NM \n");
printf("        `@@MN@MOX@@MNMN;@@MNXXOO: ,d@NbMMP.d@@OX@NO;..bb. \n");
printf("       .odMX@@XOOM@M@@XO@MMMMMMNNbNYYNNNXoNMNMOOXXNO..::;o. \n");
printf("     .ddMNOO@@XOOM@@XOONMMM@@MNXXMMo;... ..:OXO .:....  .""o. \n");
printf("    .N@@X;,M@MXOOM@OOON@MM@MXOO:::ONMNXXOXX:OOO               ""ob. \n");
printf("   .)@MP.;@@XXOOMMOOM@MNNMOO""   .OOXM@MM: :OO.        :....;o;.;Xb. \n");
printf("  .@@MXX ;X@@XXOOM@OOXXOO:o:.      :OXMNOO. ;OOO;.:     ,OXMOOXXXOOXMb \n");
printf(" ,dMOo:  oO@@MNOON@N:::      .    ,;O:"".  .dMXXO:    ,;OX@XO"":ON@M@ \n");
printf(":Y@MX:.  oO@M@NOXN@NO. ..: ,;;O;.       :.OX@@MOO;..   .OOMNMO.;XN@M@P \n");
printf(",MPOO.  oO@M@O:ON@MO;;XO;:OXMNOO;.  ,.;.;OXXN@MNXO;.. oOX@NMMN@@@@@M: \n");
printf("`. O:;;OON@@MN::XNMOOMXOOOM@@MMNXO:;XXNNMNXXXN@MNXOOOOOXNM@NM@@@M@MP \n");
printf("   :XN@MMM@M@M:  :.OON@@XXNM@M@MXOOdN@@@MM@@@@MMNNXOOOXXNNN@@M@MMMM. \n");
printf("   .oNM@MM@ONO.   :;ON@@MM@MMNNXXXM@@@@M@PY@@MMNNNNNNNNNNNM@M@M@@P. \n");
printf("  ;O:OXM@MNOOO.   .OXOONM@MNNMMXON@MM@@b. .Y@@@@@@@@@@@@@M@@MP""."" \n");
printf(" ;O.:OOXNXOOXX:   :;NMO::NMMMXOOX@MN@@@@b.:M@@@M@@@MMM@"""" \n");
printf(" :: ;OOOOOO@N;:  .ON@MO.."":""OOOO@@NNMN@@@. Y@@@MMM@@@@b \n");
printf(" :;   .:O:oX@@O;;  ;O@@XO.   ""oOOOOXMMNMNNN@MN""YMNMMM@@MMo. \n");
printf(":N:.   ..oOM@NMo.::OX@NOOo.  ;OOOXXNNNMMMNXNM@bd@MNNMMM@MM@bb \n");
printf("  @;O .  ,OOO@@@MX;;ON@NOOO.. . .:OXN@NNN@@@@@M@@@@MNXNMM@MMM@, \n");
printf("  M@O;;  :O:OX@@M@NXXOM@NOO:;;:,;;ON@NNNMM.`""@@M@@@@@MXNMMMMM@N \n");
printf("  N@NOO;:oO;O:NMMM@M@OO@NOO;O;oOOXN@NNM@@.   `Y@NM@@@@MMNNMM@MM \n");
printf("  ::@MOO;oO:::OXNM@@MXOM@OOOOOOXNMMNNNMNP      ""MNNM@@@MMMM@MP \n");
printf("    @@@XOOO.:::OOXXMNOO@@OOOOXNN@NNNNNNNN        .`YMM@@@MMM@P. \n");
printf("    MM@@M:.... O:"":ONOO@MNOOOOXM@NM@NNN@P  -hrr-     """"MM. \n");
printf("    ..MM@:     "". .OOONMOYOOOOO@MM@MNNM \n");
printf("      YM@.         :OOMN: :OOOO@MMNOXM. \n");
printf("      `:P           :oP..  "".OOM@NXNM. \n");
printf("       `.                    .:OXNP. \n");
printf("                               .. \n");

// Author -hrr-
// http://ascii.co.uk/art/dog
}

//USERINPUT
void parseUserInput(char *rawUserInput,char **userInputArray){
	if(strlen(rawUserInput) > 1 && rawUserInput[0] != ' ')
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
		printf("Woof woof!\n");
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

void redirectCommand(int commandId, int position,int maxToken,char **userInputTokenArray , char **argv){
	switch (commandId)
	{
		case 38:
  		executeInBackground(userInputTokenArray, argv);//BUG
  		break;
		case 62:
  		executeRedirect(position,userInputTokenArray, argv);
  		break;
		case 6262:
  		executeRedirectWrite(position,userInputTokenArray, argv);
  		break;
		case 60:
  		executeIndirect(position,userInputTokenArray, argv);
  		break;
		case 124:
	  	executePipe(position,maxToken,userInputTokenArray, argv);//BUG
	  	break;
		default :
			break;
	}
}

void changeDirectory(char **userInputTokenArray, int maxToken){
	char *result = malloc(maxToken);
	int i = 0;
	for(i = 1; userInputTokenArray[i] != NULL; i++){
		if(i >1)
		 strcat(result, " ");
		strcat(result, userInputTokenArray[i]);
	}
	chdir(result);
}

void basicLinuxCommands(char **userInputTokenArray,int maxToken){
	char *argv[maxToken];
	int endOrRedirectPosition = commandEndPosition(userInputTokenArray, maxToken,0);
	//printf("endOrRedirectPosition %d\n", endOrRedirectPosition);//DEBUGING
	int commandId = checkRedirect(userInputTokenArray[endOrRedirectPosition]);
  //printf("firstCommand %d\n", commandId);//DEBUGING

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
		redirectCommand(commandId,endOrRedirectPosition,maxToken, userInputTokenArray, argv);
		return;
	}

		if(fork() == 0){ //child
			execvp(argv[0],argv);
			exit(0);
		}else
			wait(0);
}

void executeInBackground(char **userInputTokenArray, char **argv){
	if(fork() == 0){ //child
		execvp(argv[0],argv);
		//fflush(stdout);
		exit(0);
	}
}

void executePipe(int position,int maxToken ,char **userInputTokenArray, char **argv){
	  char *argv2[maxToken];
	  position++;
	 // printf("position %d\n", position);//DEBUGING
		int endOrRedirectPosition = commandEndPosition(userInputTokenArray, maxToken,position) + position;
	//	printf("endOrRedirectPosition2 %d\n", endOrRedirectPosition);//DEBUGING
		int commandId = checkRedirect(userInputTokenArray[endOrRedirectPosition]);
		//printf("command %d\n", commandId);//DEBUGINGhow to use a \ in c

		int i;
		for(i = 0; i < maxToken; i++)
		{
			if(userInputTokenArray[i+ position] != NULL)
				argv2[i] = userInputTokenArray[i + position];
			else
				argv2[i] = NULL;
				//printf("argv %s\n", argv2[i]);//DEBUGING
		}

		//printf(" %d\n", commandId);//DEBUGING

		if(commandId != 0){
			redirectCommand(commandId,endOrRedirectPosition,maxToken, userInputTokenArray, argv2);
			//////////////printf("argv %d\n", commandId);//DEBUGING
			return;
		}

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

void executeIndirect(int position, char **userInputTokenArray, char **argv){
	int saved_stdout = dup(0);

	int file_id = open(userInputTokenArray[position + 1],O_RDONLY);
	close(0); // close std output  stream
	dup(file_id); // duplicate file descriptor in slot 1
	if(fork() == 0){//child
		execvp(argv[0],argv);
		exit(0);
	}
	else
		wait(0);
	close(file_id);

	// Redirect output back to stdout
	//fflush(stdout);
	dup2(saved_stdout, 0);

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
		parseUserInput(rawUserInput, userInputTokenArray);
		getCommand(userInputTokenArray, 12);//12 is the maxToken BUG
		//fflush(stdout);
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
	displayASCIIArt();
	userInputLoop();
	return 0;
}

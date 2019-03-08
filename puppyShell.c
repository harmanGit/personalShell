/**
 * Program Name: puppyShell
 * File Name: dhillonShell.c
 * Author: Harman Dhillon (2019)
 * Purpose: Basic Shell to run most linux bash shell commands
 * Not Supported: Multiple Pipes
 * Bugs(known):
 *			1. Pipes with redirects don't execute properly, although the second
 *  			 part of the pipe does execute.I have attempted to debug
 *				 this issue and don't quite understand whats going one. I have left
 *				 all the code I used to step through the user input (used to is what
 *			   see what commands are going to be executed) commented out. I think
 *				 the solution to this problem would be using dup and catching
 *				 the terminal output for the first half of the pipe. Then taking
 *				 that input and storing it, to use for the second half of the pipe.
 *				 Example Command: grep include shell.c | wc > c.txt
 *			2. When using the & and executing commands in the background, sometimes
 *				 the layout of the shell indicator gets messed up.It looks as through
 *				 a newline was entired automatically placed after shell indicator,
 *				 causing all other command to show up next to the shell indicator
 *				 and not the below it. The shell it self remains to work properly and
 *				 the & also works properly. This bug does not casue any other issues
 *				 other than the fact the visual layout is slightly messed up. I have
 *				 tried debugging this by using fflush and it made no different. I also
 *				 checked for random printf() with newlines.
 *				 Example Command: netbeans &
 *				 Steps To Replicate Error: Before entering the example command,execute
 *				 a simple commad like ls. Notice how the output of the ls command is
 *				 displayed below the shell indicator(woof~~>). Now execute the
 *				 example command. After the example command is execute close the
 *				 application (netbeans). After that use the shell again with a simple
 *				 command like ls. Notice how the output of ls is now displayed next
 *				 to the shell indicator, instead of below it.
 *
 * To Do: Implement Multiple Pipes and febug layout issue after & and pipe redirect.
 */

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
#define MAX_TOKENS 12 //max number of command arguments
#define MAX_USER_INPUT_LENGTH 128 //max chars for user input

void user_input_loop();
void shell_indicator();
void display_ascii_art();
void parse_user_input(char *raw_user_input,char **temp_user_input);
int command_ending_position(char **parsed_user_input,int position);
void execute_command(char **parsed_user_input);
int check_redirect(char *command);
void redirect_command(int command_id, int position,char **parsed_user_input, char **argv);
void change_directory(char **parsed_user_input);
void execute_linux_commands(char **parsed_user_input);
void execute_in_background(char **parsed_user_input, char **argv);
void execute_pipe(int position, char **parsed_user_input, char **argv);
void execute_redirect_write(int position, char **parsed_user_input, char **argv);
void execute_redirect_create(int position, char **parsed_user_input, char **argv);
void execute_indirect(int position, char **parsed_user_input, char **argv);
void print_out_main_array(char **temp_user_input);//USED FOR DEBUGGING

/*
* Main displays ASCII art at start up(only once at start up) and then continues
* the main loop (user_input_loop) which then is responsible all the logic of
* the puppy shell.
*/
int main(int argc, char *argv[])
{
	display_ascii_art();
	user_input_loop();
	return 0;
}

//-------------------------------SHELL DESIGN-----------------------------------
/*
* Displays a shells indicator for commands {woof~~>}
*/
void shell_indicator(){
	printf ("%s %s", SHELLNAME, INDICATOR);
}

/*
* Displays a dog image in ASCII art, which is only used once when the shells
* is first loaded. DISCLAIMER: Not My Own Art.
* Author: -hrr-
* Site: http://ascii.co.uk/art/dog
*/
void display_ascii_art(){
printf("                                        :OX \n");
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

//--------------------------------USER INPUT------------------------------------
/*
* Method is used to parse and tokenize raw user input from the command line.
* the raw user input is tokenized by the spaces between them.
*
* @param raw_user_input: (Array of chars) (pointer) representing the raw user
*									   			input from the terminal using getline.
* @param temp_user_input: (Array of Strings) (pointer) Newly parsed input,
													no spaces
*/
void parse_user_input(char *raw_user_input,char **temp_user_input){
	//checking if there is any raw user input before removing the trailing newline
	if(strlen(raw_user_input) > 1 && raw_user_input[0] != ' ')
		raw_user_input[strlen(raw_user_input) - 1] = '\0'; //removing newline

	int i = 0;
	char *temp = strtok (raw_user_input, " ");// tokenize first space

	//tokenizing the entire raw input
	while (temp != NULL)
	{
		temp_user_input[i++] = temp;
		temp = strtok (NULL, " ");
	}
}

//-----------------------------PARSING COMMANDS---------------------------------
/*
* Method is used find out where the a single linux command ends. A end is
* determined by &, |, <, >, >>, and a null value.
*
* @param parsed_user_input: (Array of Strings)(pointer) represents the entire
* 													all parsed commands
* @param position: (int) start position in the parsed_user_input.
* @return int: representing where a single command ends
*/
int command_ending_position(char **parsed_user_input, int position){
	int command_end = 0;
	int i;
	for(i = position; parsed_user_input[i] != NULL &&
		   check_redirect(parsed_user_input[i]) == 0; i++)
				 command_end++;

	return command_end;
}

/*
* Method is used execute shell commands. Exit and cd are built in and the
* rest of the commands are executed with execvp using the
* execute_linux_commands method.
*
* @param parsed_user_input: (Array of Strings)(pointer) represents the entire
* 													all parsed commands.
*/
void execute_command(char **parsed_user_input){
	char *first_command = parsed_user_input[0];//getting users first command

	if (strcmp(first_command, "exit") == 0)//exiting if first command is exit
	{
		printf("Woof woof!\n");
		exit(0);
	}
	else if (strcmp(first_command, "cd") == 0)//executing built in change_directory
          	change_directory(parsed_user_input);
	else//executing all other shell commands
		  execute_linux_commands(parsed_user_input);

}

/*
* Method is used check if the given command is special,also based of the given
* command a unique value is returns(I call the command id.) Normal commands
* return a zero.Special commands:  &, |, <, >, >>
*
* @param command: (Array of Chars)(pointer) represents single command.
* @return int: represents a special commands decimal value (command id),
* 						if the given command isn't special or is null then a 0 is return.
*/
int check_redirect(char *command){

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

/*
* Method is used execute special shell commands based of the command id recieved
*
* @param command_id: (int) special commands decimal value
* @param parsed_user_input: (Array of Strings)(pointer) represents the entire
* 														all parsed commands.
* @param position: (int) start position in the parsed_user_input.
*/
void redirect_command(int command_id, int position,char **parsed_user_input,
	 char **argv){
	switch (command_id)
	{
		case 38:
  		execute_in_background(parsed_user_input, argv);//BUG messes up lay out
  		break;
		case 62:
  		execute_redirect_create(position,parsed_user_input, argv);
  		break;
		case 6262:
  		execute_redirect_write(position,parsed_user_input, argv);
  		break;
		case 60:
  		execute_indirect(position,parsed_user_input, argv);
  		break;
		case 124://BUG complex mulit pipes don't execute properly
	  	execute_pipe(position,parsed_user_input, argv);
	  	break;
		default :
			break;
	}
}

//-----------------------------SHELL COMMANDS-----------------------------------
/*
* Method is used execute a built in change directory
*
* @param parsed_user_input: (Array of Strings)(pointer) represents the entire
* 														all parsed commands.
*/
void change_directory(char **parsed_user_input){
	char *path = malloc(MAX_TOKENS);//string represents the file path
	int i = 0;
	//Going through the array of commands and combining them together
	// (seperated by spaces) to form a string. Therefore allowing directory names
	// to be "asdf asdf asdf" can still work.
	for(i = 1; parsed_user_input[i] != NULL; i++){
		if(i >1)
		 strcat(path, " ");//adding spaces
		strcat(path, parsed_user_input[i]);
	}
	chdir(path);//execute change directory
}

/*
* Method is used execute almost all linux commands, including the basics ones
* the more complex ones like |, &, redirect, and indirect
*
* @param parsed_user_input: (Array of Strings)(pointer) represents the entire
* 														all parsed commands.
*/
void execute_linux_commands(char **parsed_user_input){
	char *argv[MAX_TOKENS];//arguments to used in execvp

	//command_ending_position() returns the position of where a single command
	//ends, the 0 is passed as the parameter represents the starting position
	int end_or_redirect_position = command_ending_position(parsed_user_input,0);
	//printf("end_or_redirect_position %d\n", end_or_redirect_position);//DEBUGING
	//finding if the end position is a null or a special command
	int command_id = check_redirect(parsed_user_input[end_or_redirect_position]);
  //printf("first_command %d\n", command_id);//DEBUGING

	int i;//creating a argv array(array of commands), to be used in execvp
	for(i = 0; i < MAX_TOKENS; i++)
	{
		if(i < end_or_redirect_position)
			argv[i] = parsed_user_input[i];
		else
			argv[i] = NULL;
			//printf("argv %s\n", argv[i]);//DEBUGING
	}
	// if the command is special, then finding method and executing it accordingly
	if(command_id != 0){
		redirect_command(command_id,end_or_redirect_position, parsed_user_input, argv);
		return;
	}
		//Forking to execute shell command
		if(fork() == 0){ //child
			execvp(argv[0],argv);//executing the argument itself
			exit(0);
		}else
			wait(0);//waiting for the child to finish executing
}

/*
* Method is used execute a command in the background, it can be triggered the &.
*
* @param parsed_user_input: (Array of Strings)(pointer) represents the entire
* 														all parsed commands.
* @param argv: (Array of Strings)(pointer) representing all the arguments(commands)
*              for	the execvp().
*/
void execute_in_background(char **parsed_user_input, char **argv){
	//Forking to execute shell command
	if(fork() == 0){ //child
		execvp(argv[0],argv);//executing the argument itself
		//fflush(stdout);//DEBUGGING layout issue caused by this method
		exit(0);//making sure no Zombies are created
	}
	//NOT waiting for the child to finish executing
}

/*
* Method is used execute multiple commands("pipes"), can be triggered the |.
*
* @param parsed_user_input: (Array of Strings)(pointer) represents the entire
* 														all parsed commands.
* @param argv: (Array of Strings)(pointer) representing all the arguments(commands)
*              for	the execvp().
* @param position: (int) start position in the parsed_user_input.
*/
void execute_pipe(int position ,char **parsed_user_input, char **argv){
	  char *argv2[MAX_TOKENS];//arguments to used in execvp
	  position++;//position is the command itself, so moving passed it
	  // printf("position %d\n", position);//DEBUGING
	  // command_ending_position() returns the position of where a single command
	  // ends, the position parameter is added to the position returned by the
	  // command_ending_position() to find out the new end of the command
		int end_or_redirect_position = command_ending_position(parsed_user_input,
			position) + position;
		//	printf("end_or_redirect_position2 %d\n", end_or_redirect_position);//DEBUGING
		//finding if the end position is a null or a special command
		int command_id = check_redirect(parsed_user_input[end_or_redirect_position]);
		//printf("command %d\n", command_id);//DEBUGING

		int i;//creating a argv array(array of commands), to be used in execvp
		for(i = 0; i < MAX_TOKENS; i++)
		{
			if(parsed_user_input[i+ position] != NULL)
				argv2[i] = parsed_user_input[i + position];
			else
				argv2[i] = NULL;
				//printf("argv %s\n", argv2[i]);//DEBUGING
		}

		//printf(" %d\n", command_id);//DEBUGING
	 // if the command is special, then finding method and executing it accordingly
		if(command_id != 0){
			redirect_command(command_id,end_or_redirect_position,
				 parsed_user_input, argv2);
			//printf("argv %d\n", command_id);//DEBUGING
			return;
		}

	   int pipefd[2];
	   pipe(pipefd);

	   if(fork() == 0){//child
	      close(pipefd[0]);//close the read end of the pipe
	      dup2(pipefd[1], 1);// connects write end of pipe to stdout
	      execvp(argv[0],argv);//the first half of the pipe, writes to pipe
	   } else if(fork() == 0){//child
	      close(pipefd[1]); //close write end of pipe
	      dup2(pipefd[0], 0);// connects write end of pipe to stdout
				//executing the second half of the pipe,reads from pipe
	      execvp(argv2[0],argv2);
	   }

	   close(pipefd[0]);
	   close(pipefd[1]);
	   wait(NULL);
	   wait(NULL);
}

/*
* Method is used redirect the output of a command to a file which is created
* with the name thats is user defined. It can be triggered the >.
*
* @param parsed_user_input: (Array of Strings)(pointer) represents the entire
* 														all parsed commands.
* @param argv: (Array of Strings)(pointer) representing all the arguments(commands)
*              for	the execvp().
* @param position: (int) start position in the parsed_user_input.
*/
void execute_redirect_write(int position, char **parsed_user_input,
	 char **argv){
	int saved_stdout = dup(1);

	int file_id = open(parsed_user_input[position + 1], O_APPEND | O_WRONLY, 0666);
	close(1); // close std output  stream
	dup(file_id); // duplicate file descriptor in slot 1
	//Forking to execute shell command
	if(fork() == 0){//child
		execvp(argv[0],argv);//executing the argument itself
		exit(0);//making sure no Zombies are created
	}
	else
		wait(0);//waiting for the child to finish executing
	close(file_id);

	// Redirect output back to stdout
	fflush(stdout);
	dup2(saved_stdout, 1);
}

/*
* Method is used redirect the output of a command to a file, where the commands
* output is appends to the file name thats user defined.
*  It can be triggered the >>.
*
* @param parsed_user_input: (Array of Strings)(pointer) represents the entire
* 														all parsed commands.
* @param argv: (Array of Strings)(pointer) representing all the arguments(commands)
*              for	the execvp().
* @param position: (int) start position in the parsed_user_input.
*/
void execute_redirect_create(int position, char **parsed_user_input,
	 char **argv){
		//Output from terminal is redirect to a file.
		int saved_stdout = dup(1);

		int file_id = open(parsed_user_input[position + 1], O_CREAT | O_WRONLY, 0666);
		close(1); // close std output  stream
		dup(file_id); // duplicate file descriptor in slot 1
		//Forking to execute shell command
		if(fork() == 0){//child
			execvp(argv[0],argv);//executing the argument itself
			exit(0);//making sure no Zombies are created
		}
		else
			wait(0);//waiting for the child to finish executing
		close(file_id);

		// Redirect output back to stdout
		fflush(stdout);
		dup2(saved_stdout, 1);
}

void execute_indirect(int position, char **parsed_user_input, char **argv){
	//Contents of a file are being redirected to the terminal
	int saved_stdout = dup(0);

	int file_id = open(parsed_user_input[position + 1],O_RDONLY);
	close(0); // close std output  stream
	dup(file_id); // duplicate file descriptor in slot 1
	//Forking to execute shell command
	if(fork() == 0){//child
		execvp(argv[0],argv);//executing the argument itself
		exit(0);//making sure no Zombies are created
	}
	else
		wait(0);//waiting for the child to finish executing
	close(file_id);

	// Redirect output back to file
	//fflush(stdout);
	dup2(saved_stdout, 0);
}

//--------------------------------CORE LOOPS------------------------------------
/*
* Method is the main for the entire shell. This method is responsible for
* accessing all the logic of the puppy shell. Core Functions: display
* shell indicator, get user input,then parse user input and store it, lastly
* executing commands based off the parsed user input.
*/
void user_input_loop(){
	size_t max_user_input_length = MAX_USER_INPUT_LENGTH;//max chars for user input
	char *parsed_user_input[MAX_TOKENS];//max number of command arguments
	char *raw_user_input;// stores raw user input from getline

	do {
	  shell_indicator();//displaying shell indicator (woof~~>)
		//setting everything in the parsed_user_input array to null
		memset(&parsed_user_input[0],'\0',MAX_USER_INPUT_LENGTH);
		//getting userinput from the terminal
	 	getline (&raw_user_input, &max_user_input_length, stdin);
		//parsing user input into parsed_user_input, a array of strings representing
		//commands
		parse_user_input(raw_user_input, parsed_user_input);
		//executing user all user commands
		execute_command(parsed_user_input);
		//fflush(stdout);//DEBUGGING layout issue
		//print_out_main_array(parsed_user_input);//Used general DEBUGING

	} while(1);//loop runs as long as the exit command is entered
	//deallocating memory
	free(parsed_user_input);
	free(raw_user_input);
	free(exit);
}
//-------------------------------DEBUG LOOPS------------------------------------
/*
* Method is used to print all stored commands from the parsed_user_input
* array into the terminal for debugging.
*/
void print_out_main_array(char **parsed_user_input){
	int i;
	for(i = 0; parsed_user_input[i] != NULL; i++)
  		printf("MAIN ARRAY: %s\n", parsed_user_input[i]);
}

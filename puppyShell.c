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
#define MAX_TOKENS 12
#define MAX_USER_INPUT_LENGTH 128

void user_input_loop();
void shell_indicator();
void display_ascii_art();
void parse_user_input(char *raw_user_input,char **temp_user_input);
int command_ending_position(char **parsed_user_input,int position);
void execute_command(char **parsed_user_input);
int check_redirect(char *command);
void redirect_command(int command_id, int position,char **parsed_user_input , char **argv);
void change_directory(char **parsed_user_input);
void execute_linux_commands(char **parsed_user_input);
void execute_in_background(char **parsed_user_input, char **argv);
void execute_pipe(int position, char **parsed_user_input, char **argv);
void execute_redirect_write(int position, char **parsed_user_input, char **argv);
void execute_redirect_create(int position, char **parsed_user_input, char **argv);
void execute_indirect(int position, char **parsed_user_input, char **argv);
void print_out_main_array(char **temp_user_input);//USED FOR DEBUGGING

//-------------------------------SHELL DESIGN-----------------------------------
void shell_indicator(){
	printf ("%s %s", SHELLNAME, INDICATOR);
}

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
void parse_user_input(char *raw_user_input,char **temp_user_input){
	if(strlen(raw_user_input) > 1 && raw_user_input[0] != ' ')
		raw_user_input[strlen(raw_user_input) - 1] = '\0'; //removing newline

	int i = 0;
	char *temp = strtok (raw_user_input, " ");

	while (temp != NULL)
	{
		temp_user_input[i++] = temp;
		temp = strtok (NULL, " ");
	}
}

//-----------------------------PARSING COMMANDS---------------------------------
int command_ending_position(char **parsed_user_input, int position){
	int command_end = 0;
	int i;
	for(i = position; parsed_user_input[i] != NULL &&
		   check_redirect(parsed_user_input[i]) == 0; i++)
				 command_end++;

	return command_end;
}

void execute_command(char **parsed_user_input){
	char *first_command = parsed_user_input[0];

	if (strcmp(first_command, "exit") == 0)
	{
		printf("Woof woof!\n");
		exit(0);
	}
	else if (strcmp(first_command, "cd") == 0)
          	change_directory(parsed_user_input);
	else
		  execute_linux_commands(parsed_user_input);

}

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

void redirect_command(int command_id, int position,char **parsed_user_input,
	 char **argv){
	switch (command_id)
	{
		case 38:
  		execute_in_background(parsed_user_input, argv);//BUG
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
		case 124:
	  	execute_pipe(position,parsed_user_input, argv);//BUG
	  	break;
		default :
			break;
	}
}

//-----------------------------SHELL COMMANDS-----------------------------------
void change_directory(char **parsed_user_input){
	char *result = malloc(MAX_TOKENS);
	int i = 0;
	for(i = 1; parsed_user_input[i] != NULL; i++){
		if(i >1)
		 strcat(result, " ");
		strcat(result, parsed_user_input[i]);
	}
	chdir(result);
}

void execute_linux_commands(char **parsed_user_input){
	char *argv[MAX_TOKENS];
	int end_or_redirect_position = command_ending_position(parsed_user_input,0);
	//printf("end_or_redirect_position %d\n", end_or_redirect_position);//DEBUGING
	int command_id = check_redirect(parsed_user_input[end_or_redirect_position]);
  //printf("first_command %d\n", command_id);//DEBUGING

	int i;
	for(i = 0; i < MAX_TOKENS; i++)
	{
		if(i < end_or_redirect_position)
			argv[i] = parsed_user_input[i];
		else
			argv[i] = NULL;
			//printf("argv %s\n", argv[i]);//DEBUGING
	}

	if(command_id != 0){
		redirect_command(command_id,end_or_redirect_position, parsed_user_input, argv);
		return;
	}

		if(fork() == 0){ //child
			execvp(argv[0],argv);
			exit(0);
		}else
			wait(0);
}

void execute_in_background(char **parsed_user_input, char **argv){
	if(fork() == 0){ //child
		execvp(argv[0],argv);
		//fflush(stdout);
		exit(0);
	}
}

void execute_pipe(int position ,char **parsed_user_input, char **argv){
	  char *argv2[MAX_TOKENS];
	  position++;
	 // printf("position %d\n", position);//DEBUGING
		int end_or_redirect_position = command_ending_position(parsed_user_input,
			position) + position;
	//	printf("end_or_redirect_position2 %d\n", end_or_redirect_position);//DEBUGING
		int command_id = check_redirect(parsed_user_input[end_or_redirect_position]);
		//printf("command %d\n", command_id);//DEBUGINGhow to use a \ in c

		int i;
		for(i = 0; i < MAX_TOKENS; i++)
		{
			if(parsed_user_input[i+ position] != NULL)
				argv2[i] = parsed_user_input[i + position];
			else
				argv2[i] = NULL;
				//printf("argv %s\n", argv2[i]);//DEBUGING
		}

		//printf(" %d\n", command_id);//DEBUGING

		if(command_id != 0){
			redirect_command(command_id,end_or_redirect_position,
				 parsed_user_input, argv2);
			//////////////printf("argv %d\n", command_id);//DEBUGING
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

void execute_redirect_write(int position, char **parsed_user_input,
	 char **argv){
	int saved_stdout = dup(1);

	int file_id = open(parsed_user_input[position + 1], O_APPEND | O_WRONLY, 0666);
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

void execute_redirect_create(int position, char **parsed_user_input,
	 char **argv){
		int saved_stdout = dup(1);

		int file_id = open(parsed_user_input[position + 1], O_CREAT | O_WRONLY, 0666);
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

void execute_indirect(int position, char **parsed_user_input, char **argv){
	int saved_stdout = dup(0);

	int file_id = open(parsed_user_input[position + 1],O_RDONLY);
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

//--------------------------------CORE LOOPS------------------------------------
void user_input_loop(){
	size_t max_user_input_length = MAX_USER_INPUT_LENGTH;
	char *parsed_user_input[MAX_TOKENS];
	char *raw_user_input;
	char *exit = "exit";

	do {
	  shell_indicator();
		//setting everything in the array eqaul to null
		memset(&parsed_user_input[0],'\0',MAX_USER_INPUT_LENGTH);
	 	getline (&raw_user_input, &max_user_input_length, stdin);
		parse_user_input(raw_user_input, parsed_user_input);
		execute_command(parsed_user_input);//12 is the maxToken BUG
		//fflush(stdout);
		//print_out_main_array(parsed_user_input);//For DEBUGING

	} while(1);//loop runs as long as its not exited
	free(parsed_user_input);
	free(raw_user_input);
	free(exit);
}

void print_out_main_array(char **parsed_user_input){//USED FOR DEBUGGING
	int i;
	for(i = 0; parsed_user_input[i] != NULL; i++)
  		printf("MAIN ARRAY: %s\n", parsed_user_input[i]);
}

int main(int argc, char *argv[])
{
	display_ascii_art();
	user_input_loop();
	return 0;
}

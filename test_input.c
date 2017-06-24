#include <stdio.h>
#include "input.h"
#include <stdlib.h>
#include <string.h>

#define ID_NEWCOM 2
#define ID_ECHO 3
#define ID_BARRELROLL 4
#define ID_READFILE 5
#define ID_COUNT 5

void init(Commands *c){
	//I find it useful to keep the initialisation of commands as a seperate function
	//commands are defined here, the logic is in the main loop
	c = append_command_list(c, ID_NEWCOM, "newcom", "creating new command", "creates a new command with the response text of your first argument, the help text of the second, and a defined id");
	c = append_command_list(c, ID_ECHO, "echo", "", "echos back the text you said in the first argument. will repeat the specified number of times in the second (if there is a second argument");
	c = append_command_list(c, ID_READFILE, "readfile", "", "reads a file specified and prints it to stdout");
}

int main(){
	//inbuilt function to initialise a command list. Includes the functions exit and help
	Commands *command_list = init_command_list();
	init(command_list);

	//Initialise the argument list
	Arg *arguments = init_arg_list();

	int id_count = ID_COUNT;
	
	while(1){
		arguments = get_input(command_list, arguments);
		if(arguments == NULL) return 1;
		int id = arguments->id;
		if(id == ID_BARRELROLL){
			printf(" _  _  _\n/T\\/O\\/M\\\n\\_/\\_/\\_/\n");
		}else if(id == ID_NEWCOM){
			//multiple arguments can be used. 
			//arguments->arg refers to the command entered, in this case it would be "newcom"
			//arguemnts->next->arg would refer to the first argument entered, for example if "newcom hello hi greetinger" was entered then "hello" would be arguments->next->arg
			//this method of referencing the next can be done continuosly until arguments->next will result in a NULL indicating that there are no more arguments entered
			if(arguments && arguments->next && arguments->next->next && arguments->next->next->next){
				command_list = append_command_list(command_list, id_count + 1,arguments->next->arg, arguments->next->next->arg, arguments->next->next->next->arg);
				id_count ++;
			}else{
				printf("ERROR: newcom must be in the form 'newcom COMMAND RESPONSE HELP_TEXT'\n");
			}
		}else if(id == ID_ECHO){
			if(arguments && arguments->next){
				if(arguments->next->next && atoi(arguments->next->next->arg) > 0){
					for(int i = 0; i < atoi(arguments->next->next->arg); i++){
						printf("%s\n",arguments->next->arg);
					}
				}else{
					printf("%s\n",arguments->next->arg);
				}
			}else{
				printf("ERROR: echo must be in the form 'echo STRING [optional int]'\n");
			}
		}else if(id == ID_READFILE){
			char loc[MAX_COMMAND_LENGTH] = {0};
			if(arguments && arguments->next){
				strcpy(loc, arguments->next->arg);
				FILE *f = fopen(loc, "r");
				if(f){
					char buffer[MAX_COMMAND_LENGTH] = {0};
					while(fgets(buffer,MAX_COMMAND_LENGTH, f) != NULL){
						printf("%s",buffer);
					}
				}else{
					printf("Error: The file %s does not exist\n",loc);
				}
			}else{
				printf("Specify a file for %s\n",arguments->arg);
			}
		}
	}
	return 0;
}

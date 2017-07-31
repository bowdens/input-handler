#include <stdio.h>
#include "libtalaris.h"
#include <stdlib.h>
#include <string.h>

#define ID_NEWCOM 2
#define ID_ECHO 3
#define ID_BARRELROLL 4
#define ID_READFILE 5
#define ID_DELCOM 6
#define ID_SSECRET 7
#define ID_FORCEPRINT 8
#define ID_COUNT 8

void init(Commands *c){
	//I find it useful to keep the initialisation of commands as a seperate function
	//commands are defined here, the logic is in the main loop
    c = append_command_list(c, ID_NEWCOM, "newcom", "", "creates a new command with the response text of your first argument, the help text of the second, and a defined id", COM_SHOWN);
	c = append_command_list(c, ID_ECHO, "echo", "", "echos back the text you said in the first argument. will repeat the specified number of times in the second (if there is a second argument", COM_SHOWN);
    c = append_command_list(c, ID_BARRELROLL, "barrel", "doing a barrel roll", "easter egg!", COM_SHOWN);
	c = append_command_list(c, ID_READFILE, "readfile", "", "reads a file specified and prints it to stdout", COM_SHOWN);
    c = append_command_list(c, ID_DELCOM, "delcom", "", "deletes a command with a specified ID", COM_SHOWN);
    c = append_command_list(c, ID_SSECRET, "ssecre", "You aren't supposed to see this", "If you are seeing this there is an error", COM_SILENT);
    c = append_command_list(c, ID_FORCEPRINT, "forceprint", "", "Toggle the state of the forced print", COM_SHOWN);
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

			if(arguments && arguments->next && arguments->next->next && arguments->next->next->next){
                id_count++;
				command_list = append_command_list(command_list, id_count,arguments->next->arg, arguments->next->next->arg, arguments->next->next->next->arg, COM_SHOWN);
				printf("command '%s' created\n",command_str(id_count, command_list));
			}else{
				printf(C_R"ERROR"C_W": newcom must be in the form 'newcom COMMAND RESPONSE HELP_TEXT'\n");
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
				printf(C_R"ERROR"C_W": echo must be in the form 'echo STRING [optional int]'\n");
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
					printf(C_R"ERROR"C_W": The file %s does not exist\n",loc);
				}
			}else{
				printf("Specify a file for %s\n",arguments->arg);
			}

        }else if(id == ID_DELCOM){

            if(arguments && arguments->next){
                if(atoi(arguments->next->arg) == 0){
                    command_list = delete_command_char(command_list, arguments->next->arg, 1);
                }else{
                    int specId = atoi(arguments->next->arg);
                    command_list = delete_command(command_list, specId,1);
                }
            }else{
                printf(C_R"ERROR"C_W": delcom must be in the form 'delcom ID'\n");
            }

        }else if(id == ID_FORCEPRINT){
            set_allowForcePrint(!get_allowForcePrint());
            if(get_allowForcePrint() == PRINT_FORCE){
                printf("Set print to force\n");
            }else{
                printf("Set print to silent\n");
            }
        }
	}
	return 0;
}

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
#define ID_VERBOSEMODE 9
#define ID_TOGGLEHELP 10
#define ID_COUNT 10

void init(Commands *c){
	//I find it useful to keep the initialisation of commands as a seperate function
	//commands are defined here, the logic is in the main loop
    c = append_command_list(c, ID_NEWCOM, "newcom", "", "creates a new command with the response text of your first argument, the help text of the second, and a defined id", COM_SHOWN);
	c = append_command_list(c, ID_ECHO, "echo", "", "echos back the text you said in the first argument. will repeat the specified number of times in the second (if there is a second argument", COM_SHOWN);
    c = append_command_list(c, ID_BARRELROLL, "barrel", "doing a barrel roll", "easter egg!", COM_HIDDEN);
	c = append_command_list(c, ID_READFILE, "readfile", "", "reads a file specified and prints it to stdout", COM_SHOWN);
    c = append_command_list(c, ID_DELCOM, "delcom", "", "deletes a command with a specified ID", COM_SHOWN);
    c = append_command_list(c, ID_SSECRET, "ssecret", "You aren't supposed to see this", "If you are seeing this there is an error", COM_SILENT);
    c = append_command_list(c, ID_FORCEPRINT, "forceprint", "", "Toggle the state of the forced print", COM_SHOWN);
    c = append_command_list(c, ID_VERBOSEMODE, "verbosemode", "", "switch between verbose mode. verbose mode 0 = default, verbose mode 1 = verbose, verbose mode 2 = extra verbose", COM_SHOWN);
    c = append_command_list(c, ID_TOGGLEHELP, "togglehelp", "", "toggle between help functions", COM_SHOWN);
}

void helpf(Commands *c, int force){
    //you can define your own help and exit commands like this
    //to define a new help command it needs a prototype of void function(Commands, int) since that is what the default uses
    printf("Write code here to define your own help function. Due to the inbuilt help functions prototype any help function you make must also use this prototype\n");
}

void exitf(void){
    //to define an exit function it needs the prototype of void functioN(void).
    printf("Exiting\n.");
    printf("specify some code here\n");
    exit(EXIT_SUCCESS);
}

int main(){
    //printf("Edited\n");
	//inbuilt function to initialise a command list. Includes the functions exit and help
	Commands *command_list = init_command_list();
	init(command_list);

	//Initialise the argument list
	Arg *arguments = init_arg_list();

	int id_count = ID_COUNT;

    //you can set your own help and exit functions here if required
    set_lt_help_function(helpf);
    set_lt_exit_function(exitf);

    //to reset the functions to default:
    set_lt_help_function(lt_help);
    set_lt_exit_function(lt_exit);


	while(1){
		//this is the main loop of the program

        //the following line gets the command that the user enters, stored in an Arg data structure
        //the Arg data structure stores the command entered as arguments->argument, and subsequent words seperated by spaces in subsequent arguments->next->argument
        arguments = get_input(command_list, arguments);

		if(arguments == NULL){
            //to catch if arguments is null (this should never happen)
            fprintf(stderr,"Program ended unexpectedly. Error: arguments were NULL\n");
            exit(EXIT_FAILURE);
        }

        //the id of the first argument is the id of the command entered by the user. ID_NONE refers to a command entered that could not be matched with one in the program
		int id = arguments->id;

        //the following multiway selection will execute code based on which argument was entered
        //
        //TODO: change this code to a select statement; put it in its own function
		if(id == ID_BARRELROLL){

            //example of a hidden command that can be executed but is not shown in help
            //notice that ssecret is defined as a secret command in the initialisation function but the logic of it is not specified here
			printf(" _  _  _\n/T\\/O\\/M\\\n\\_/\\_/\\_/\n");

        }else if(id == ID_NEWCOM){

            //creates a new command using an id
			if(arguments && arguments->next && arguments->next->next && arguments->next->next->next){
                id_count++;
				command_list = append_command_list(command_list, id_count,arguments->next->arg, arguments->next->next->arg, arguments->next->next->next->arg, COM_SHOWN);
				printf("command '%s' created\n",command_str(id_count, command_list));
			}else{
				printf(C_R"ERROR"C_W": newcom must be in the form 'newcom COMMAND RESPONSE HELP_TEXT'\n");
            }

        }else if(id == ID_ECHO){

            //prints the first argument specified, as a number of times specified by the user
            if(arguments && arguments->next){
				if(arguments->next->next && atoi(arguments->next->next->arg) > 0){
					for(int i = 0; i < atoi(arguments->next->next->arg); i++){
						printf("%s\n",arguments->next->arg);
					}
				}else{
					printf("%s\n",arguments->next->arg);
				}
			}else{
                //failing arguments->next implies that no argument was added after "echo"
				printf(C_R"ERROR"C_W": echo must be in the form 'echo STRING [optional int]'\n");
			}

        }else if(id == ID_READFILE){

            //reads a file and prints it to stdout
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

            //deletes a command using the delete_command_char (if a string was entered) or using delete_command (if a number was entered)
            if(arguments && arguments->next){
                if(atoi(arguments->next->arg) == 0){
                    command_list = delete_command_char(command_list, arguments->next->arg);
                }else{
                    int specId = atoi(arguments->next->arg);
                    command_list = delete_command(command_list, specId);
                }
            }else{
                printf(C_R"ERROR"C_W": delcom must be in the form 'delcom ID'\n");
            }

        }else if(id == ID_FORCEPRINT){

            //setting the global variable of allowForcePrint
            set_allowForcePrint(!get_allowForcePrint());

        }else if(id == ID_VERBOSEMODE){

            if(arguments->next == NULL){
                //arguments->next being NULL implies that only the command "verbosemode" was entered without any subsequent arguments
                printf(C_R"ERROR"C_W": Please specify a verbose mode, 0,1, or 2\n");
            }else if(atoi(arguments->next->arg) != 0){
                //the user entered an argument for verbosemode which was not 0
                set_lt_verbose(atoi(arguments->next->arg));
            }else{
                if(arguments->next->arg[0] == '0' && arguments->next->arg[1] == '\0'){
                    //the user entered an arugment for verbosemode which was 0
                    set_lt_verbose(0);
                }else{
                    //the user specified a non unmeric argument for verbosemode
                    printf(C_R"ERROR"C_W": Please specific a numeric value, 0, 1, or 2\n");
                }
            }

        }else if(id == ID_TOGGLEHELP){

            //redefines the help function to be whatever it currently is not
            set_lt_help_function(get_lt_help_function() == lt_help?helpf:lt_help);
            printf("Set help function to the %s function\n",get_lt_help_function() == lt_help ? "default" : "program specified");
        }
	}
	return 0;
}

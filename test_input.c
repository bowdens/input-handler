#include <stdio.h>
#include "input.h"
#include <stdlib.h>
#include <string.h>

#define ID_NEWCOM 2
#define ID_ECHO 3
#define ID_BARRELROLL 4
#define ID_COUNT 4
void init(Commands *c){
	c = append_command_list(c, ID_BARRELROLL, "barrelroll", "doing a barrel roll", "just for fun");
	c = append_command_list(c, ID_NEWCOM, "newcom", "creating new command", "creates a new command with the response text of your first argument, the help text of the second, and a defined id");
	c = append_command_list(c, ID_ECHO, "echo", "", "echos back the text you said in the first argument. will repeat the specified number of times in the second (if there is a second argument");
}

int main(){
	//printf("main started\n");
	Commands *command_list = malloc(sizeof(Commands));
	//print_list_commands(command_list);
	//printf("initialising command_list first element\n");
	command_list = init_command_list();
	//print_list_commands(command_list);
        //printf("initialising command list\n");
	init(command_list);
	//print_list_commands(command_list);
	//printf("Entering loop\n");
	Arg *arguments = malloc(sizeof(Arg));
	int id_count = ID_COUNT;
	while(1){
		//printf("loop iteration\n");
		arguments = get_input(command_list, arguments);
		//printf("arguments->arg = %s, arguments->id = %d\n",arguments->arg, arguments->id);
		if(arguments == NULL) return 1;
		int id = arguments->id;
		//printf("id = %d\n",id);
		if(id == ID_BARRELROLL){
			printf(" _  _  _\n/T\\/O\\/M\\\n\\_/\\_/\\_/\n");
		}else if(id == ID_NEWCOM){
			char arg0[MAX_COMMAND_LENGTH] = {0};
			char arg1[MAX_RESPONSE_LENGTH] = {0};
			char arg2[MAX_RESPONSE_LENGTH] = {0};
			if(arguments->next){
				strcpy(arg0, arguments->next->arg);
				if(arguments->next->next){
					strcpy(arg1, arguments->next->next->arg);
					if(arguments->next->next->next){
						strcpy(arg2, arguments->next->next->next->arg);
					}
				}
			}
			command_list = append_command_list(command_list, id_count + 1,arg0, arg1, arg2);
			id_count ++;
		}else if(id == ID_ECHO){
			char arg1[MAX_COMMAND_LENGTH] = {0};
			int arg2 = 1;
			if(arguments->next){
				strcpy(arg1, arguments->next->arg);
				if(arguments->next->next){
					if(atoi(arguments->next->next->arg)){
						arg2 = atoi(arguments->next->next->arg);
					}
				}
			}
			for(int i = 0; i < arg2; i ++){
				printf("%s\n",arg1);
			}
		}
	}
	return 0;
}

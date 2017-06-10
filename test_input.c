#include <stdio.h>
#include "input.h"
#include <stdlib.h>

#define ID_TEST 2
#define ID_THREE 3
#define ID_BARRELROLL 4

void init(Commands *c){
	c = append_command_list(c, ID_TEST, "test", "", "tests an input");
	c = append_command_list(c, ID_THREE, "three", "three was entered", "tests another input");
	c = append_command_list(c, ID_BARRELROLL, "barrel roll", "doing a barrel roll", "just for fun");
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
	while(1){
		//printf("loop iteration\n");
		arguments = get_input(command_list);
		if(arguments == NULL) return 1;
		int id = arguments->id;
		if(id == ID_END){
			return 1;
		}else if(id == ID_TEST){
			printf("executing two code\n");
		}else if(id == ID_BARRELLROLL){
			printf("executing three code\n");
		}
	}
	return 0;
}

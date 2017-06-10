#include "input.h"
#include <stdlib.h>
#include <assert.h>
#define ID_EXIT -2
#define ID_HELP -3

void print_list_commands(Commands *c){
	printf("[");
	while(c){
		printf("%s, %s,%s, %d", c->command, c->response, c->help_text, c->id);
		if(c->next){
			printf(";\n");
		}
	c = c->next;
	}
	printf("\n]\n");
}

void sanatise_command(char command[MAX_COMMAND_LENGTH]){
	for(int i = 0; i < MAX_COMMAND_LENGTH && command[i] != '\0'; i ++){
		if(command[i] == '\n'){
			command[i] = '\0';
			return;
		}
	}
}

int handle_input(char *command, Commands *c){
	//printf("handle_input called\n");
	if(c == NULL) return -1;
	Commands *head = c;
	while(c != NULL){
		if(strcmp(command, c->command) == 0){
			if(c->response[0] != '\0' || strcmp(c->response, "")){
				printf("%s\n",c->response);
			}
			if(c->id == ID_HELP){
				while(head){
					printf("%s\n", head->command);
					printf("    %s\n",head->help_text);
					printf("    ID: %d\n",head->id);
					head = head->next;
				}
			}
			return c->id;
		}
		c = c->next;
	}
	printf("Unknown command\n");
	return -1;
}

int get_input(Commands *c){
	//printf("get_input called\n");
	char command[MAX_COMMAND_LENGTH] = {0};
	printf("> ");
	fgets(command, MAX_COMMAND_LENGTH, stdin);
	sanatise_command(command);
	//printf("   command entered was '%s'\n",command);
	int id = handle_input(command, c);
	//printf("   id was %d\n",id);
	return id;
}

Commands *create_command_list(int id, char command[MAX_COMMAND_LENGTH], char response[MAX_RESPONSE_LENGTH], char help_text[MAX_HELP_TEXT_LENGTH]){
	//printf("    called create_command_list\n");
	Commands *c = malloc(sizeof(Commands));
	assert(c);
	//printf("    copying data\n");
	strcpy(c->command, command);
	//printf("        copied command\n");
	strcpy(c->response, response);
	//printf("        copied response\n");
	strcpy(c->help_text, help_text);
	//printf("        copied help_text\n");
	c->id = id;
	//printf("        copied id\n");
	c->next = NULL;
	//print_list_commands(c);
	return c;
}

Commands *init_command_list(){
	//printf("    init_command_list called\n");
	Commands *c = create_command_list(ID_HELP, "help", " ", "display this information");
	//printf("creating exit command\n");
	c->next = create_command_list(ID_EXIT, "exit", "exiting program", "exits the program");
	return c;
}

Commands *append_command_list(Commands *c, int id, char command[MAX_COMMAND_LENGTH], char response[MAX_RESPONSE_LENGTH], char help_text[MAX_HELP_TEXT_LENGTH]){
	if(c == NULL) return create_command_list(id, command, response, help_text);
	if(id == ID_EXIT || id == ID_HELP) return c;
	Commands *head = c;
	while(c->next != NULL){
		c = c->next;
	}
	c->next = create_command_list(id, command, response, help_text);
	//print_list_commands(head);
	return head;
}

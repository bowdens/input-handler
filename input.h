#define MAX_COMMAND_LENGTH 128
#define MAX_RESPONSE_LENGTH 4096
#define MAX_HELP_TEXT_LENGTH 4096

#define ID_EXIT -2
#define ID_HELP -3

#include <stdio.h>
#include <string.h>

typedef struct commands{
        char command[MAX_COMMAND_LENGTH];
        char response[MAX_RESPONSE_LENGTH];
	char help_text[MAX_HELP_TEXT_LENGTH];
	int id;
	struct commands *next;
} Commands;

typedef struct arg{
	char arg[MAX_COMMAND_LENGTH];
	int id;
	struct arg *next;
} Arg;

int handle_input(char *command, Commands *c, Arg *a);

Arg *get_input(Commands *c, Arg *a);

void print_list_commands(Commands *c);

Commands *create_command_list(int id, char command[MAX_COMMAND_LENGTH], char response[MAX_RESPONSE_LENGTH], char help_text[MAX_HELP_TEXT_LENGTH]);

Commands *append_command_list(Commands *c, int id, char command[MAX_COMMAND_LENGTH], char response[MAX_RESPONSE_LENGTH], char help_text[MAX_HELP_TEXT_LENGTH]);

Commands *init_command_list();

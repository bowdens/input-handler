#ifndef _TALARIS
#define _TALARIS 1

#define COM_SHOWN 1
#define COM_HIDDEN 0
#define COM_SILENT -1

#define PRINT_FORCE 1
#define PRINT_SILENT 0

#define MAX_COMMAND_LENGTH 128
#define MAX_RESPONSE_LENGTH 4096
#define MAX_HELP_TEXT_LENGTH 4096

#define LT_PROMPT_CHAR ">"

#define ID_NONE -1
#define ID_EXIT -2
#define ID_HELP -3

#define LT_VERBOSE_OFF 0
#define LT_VERBOSE_ON 1
#define LT_VERBOSE_EXTRA 2

#define C_R "\x1b[31m"
#define C_G   "\x1b[32m"
#define C_Y  "\x1b[33m"
#define C_B    "\x1b[34m"
#define C_M "\x1b[35m"
#define C_C "\x1b[36m"
#define C_W   "\x1b[0m"

#define LT_PRINT_WARN "["C_Y"WARN"C_W"]"
#define LT_PRINT_ERROR "["C_R"ERROR"C_W"]"
#define LT_PRINT_INFO "[INFO]"

typedef struct commands{
    char command[MAX_COMMAND_LENGTH];
    char response[MAX_RESPONSE_LENGTH];
	char help_text[MAX_HELP_TEXT_LENGTH];
	int id;
    int state;
	struct commands *next;
} Commands;

typedef struct arg{
	char arg[MAX_COMMAND_LENGTH];
	int id;
	struct arg *next;
} Arg;

typedef struct argStack{
    Arg *a;
    struct argStack *next;
    struct argStack *prev;
} ArgStack;

typedef struct similar{
    char command[MAX_COMMAND_LENGTH];
    struct similar *next;
} Similar;

typedef void Helpf(Commands*, int);
typedef void Exitf(Commands*, Arg*);

void set_lt_help_function(Helpf *helpf);
void set_lt_exit_function(Exitf *exitf);

Helpf *get_lt_help_function();
Exitf *get_lt_exit_function();

void lt_help(Commands *c, int force);
void lt_exit(Commands *c, Arg *a);

int handle_input(Commands *c, Arg *a);

//Arg functions
Arg *get_input(Commands *c, Arg *a);

Arg *init_arg_list();

Arg *free_arg(Arg *a);

//Command Functions
void print_list_commands(Commands *c);

Commands *create_command_list(int id, char command[MAX_COMMAND_LENGTH], char response[MAX_RESPONSE_LENGTH], char help_text[MAX_HELP_TEXT_LENGTH], int state);

Commands *append_command_list(Commands *c, int id, char command[MAX_COMMAND_LENGTH], char response[MAX_RESPONSE_LENGTH], char help_text[MAX_HELP_TEXT_LENGTH], int state);

Commands *delete_command(Commands *c, int id);

Commands *delete_command_char(Commands *c, char *command);

Commands *init_command_list();

Commands *free_comm(Commands *c);

int command_id(char *command, Commands *c);

char *command_str(int id, Commands *c);

void insert_at_end(char *first, char *second, int maxLen);

//GLOBAL VARIABLE INTERACTIONS
int get_allowDuplicateCommands(void);
void set_allowDuplicateCommands(int x);

int get_allowForcePrint(void);
void set_allowForcePrint(int x);

int get_lt_verbose(void);
void set_lt_verbose(int x);
#endif

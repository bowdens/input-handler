#include "libtalaris.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

void print_error(char *error);

//GLOBAL VARIBALES
int allowDuplicateCommands = 0;
int allowForcePrint = PRINT_SILENT;
int lt_verbose = LT_VERBOSE_OFF;

//controls whether or not duplicate commands are allowed
int get_allowDuplicateCommands(void){
    if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"allowDuplicateCommands = %s\n:",allowDuplicateCommands?"True":"False");
    return allowDuplicateCommands;
}
void set_allowDuplicateCommands(int x){
    if(lt_verbose) printf(LT_PRINT_INFO"%sallowing duplicate commands\n",allowDuplicateCommands?"":"dis");
    allowDuplicateCommands = x;
}

//force prints hidden and silent commands
int get_allowForcePrint(void){
    if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"allowForcePrint = %s\n", allowForcePrint?"True":"False");
    return allowForcePrint;
}
void set_allowForcePrint(int x){
    allowForcePrint = x;
    if(lt_verbose) printf(LT_PRINT_INFO"%sallowing force print\n", allowForcePrint?"" : "dis");
}

//verbose mode
int get_lt_verbose(void){
    if(lt_verbose) printf(LT_PRINT_INFO"verbose mode: %s\n",lt_verbose==LT_VERBOSE_ON ? "on" : "extra");
    return lt_verbose;
}
void set_lt_verbose(int x){
    if(x == LT_VERBOSE_OFF || x == LT_VERBOSE_ON || x == LT_VERBOSE_EXTRA) lt_verbose = x;
    else if(lt_verbose) print_error("lt_verbose was attempted to be set to %d. Please use LT_VERBOSE_OFF, LT_VERBOSE_ON, or LT_VERBOSE_EXTRA");
    if(lt_verbose) printf(LT_PRINT_INFO"verbose set to %s\n", lt_verbose == LT_VERBOSE_ON ? "on" : "extra");
}

void print_help(Commands *c, int force);

void lt_help(Commands *c, int force){
    print_help(c, force);
}
void lt_exit(void){
    printf("exiting the program due to user input\n");
    exit(0);
}

//declaring the help function and exit function
Helpf *lt_help_function = lt_help;
Exitf *lt_exit_function = lt_exit;

void set_lt_help_function(Helpf *helpf){
    if(helpf == NULL){
        if(lt_verbose) printf(LT_PRINT_WARN"Attempted to set helpf to NULL, overridden\n");
        return;
    }
    if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"Help function redefined as %p\n",helpf);
    lt_help_function = helpf;
}

Helpf *get_lt_help_function(void){
    return lt_help_function;
}

Exitf *get_lt_exit_function(void){
    return lt_exit_function;
}

void set_lt_exit_function(Exitf *exitf){
    if(exitf == NULL){
        if(lt_verbose) printf(LT_PRINT_WARN"Attempted to set exitf to NULL, overridden\n");
        return;
    }
    if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"Exit function redfined as %p\n",exitf);
    lt_exit_function = exitf;
}


//REGULAR FUNCTIONS

void print_no_memory(char *varName){
    printf(LT_PRINT_ERROR"Tried to allocate memory for %s but could not. Out of memory or bug in program?\n",varName);
    exit(0);
}

void check_not_null(void *ptr, char *varName){
    if(ptr == NULL) print_no_memory(varName);
}

void print_error(char *error){
    //short hand method of printing an error message
    fprintf(stderr,LT_PRINT_ERROR"%s",error);
    printf("\n");
}

void print_list_commands(Commands *c){
    //prints the list of possible commands
	printf("[");
	while(c){
		if(lt_verbose == LT_VERBOSE_EXTRA){
            printf(C_C"%s"C_W", %s,%s, %d", c->command, c->response, c->help_text, c->id);
        }else{
            printf("command = '"C_C"%s"C_W"', reponse = '%s', help_text = '%s', id = %d",c->command, c->response, c->help_text, c->id);
        }
        if(c->next){
			printf(";\n");
		}
		c = c->next;
	}
	printf("]\n");
}

int command_id(char *command, Commands *c){
    //finds the id of a command associated with a string
    if(c == NULL) return ID_NONE;
    if(strcmp(command, "") == 0) return ID_NONE;
    while(c != NULL){
        if(!strcmp(command, c->command) && c->state != COM_SILENT){
            if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"%s has id %d\n",command, c->id);
            return c->id;
        }
        c = c->next;
    }
    if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_WARN"command '%s' does not exist\n",command);
    return ID_NONE;
}

char *command_str(int id, Commands *c){
    //finds the strings associated with an id of a command

    //TODO: Update this to be in the usual syntax with Commands *c first, as with above
    if(c == NULL) return "";
    while(c != NULL){
        if(c->id == id){
            if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"command id %d is associated with command %s\n",id, c->command);
            return c->command;
        }
        c = c->next;
    }
    return "";
}


Arg *find_element(Arg *a, int index){
    //for an arg list, finds and returns the element in an arg list matching a given index

    int i = 0;
	while(a && i < index){
		a = a->next;
		i++;
	}
	if(i != index) return NULL;
	return a;
}

//command stack

void print_arg(Arg *a){
    //prints an argument list
    printf("[");
    while(a){
        printf("%s",a->arg);
        if(a->next){
            printf(",");
        }
        a = a->next;
    }
    printf("]");
}

void print_arg_stack(ArgStack *as){
    printf("[");
    while(as){
        print_arg(as->a);
        if(as->next){
            printf(",");
        }
        as = as->next;
    }
    printf("]");
}

ArgStack *create_arg_stack(Arg *a){
    //creates a new argstack and returns it

    ArgStack *as = malloc(sizeof(ArgStack));
    check_not_null(as, "new ArgStack");
    as->a = a;
    as->next = NULL;
    as->prev = NULL;
    if(lt_verbose == LT_VERBOSE_EXTRA){
        printf(LT_PRINT_INFO"created new arg stack with arguments = ");
        print_arg(a);
        printf("\n");
    }
    return as;
}

ArgStack *last_arg_stack(ArgStack *as){
    //returns the last arg in an arg stack

    if(as == NULL) return NULL;
    while(as->next) as = as->next;
    return as;
}

ArgStack *first_arg_stack(ArgStack *as){
    //returns the first argstack node in an argstack

    if(as == NULL) return NULL;
    while(as->prev) as = as->prev;
    return as;
}

ArgStack *push_arg_stack(ArgStack *a, ArgStack *b){
    //pushes an argstack to another argstack. returns the new argstack

    if(lt_verbose == LT_VERBOSE_EXTRA){
        printf(LT_PRINT_INFO"pushing argStack ");
        print_arg_stack(a);
        printf(" to argStack ");
        print_arg_stack(b);
        printf("\n");
    }
    if(a == NULL) return b;
    if(b == NULL) return a;
    a->prev = last_arg_stack(b);
    last_arg_stack(b)->next = a;
    return b;
}

//commands entered

Arg *arg_create_list(Commands *c, char argument[MAX_COMMAND_LENGTH], int id, int allowNoString){
	//creates a new argument node
    //if allowNoString is 0 then if argument == "" then it will return the command as "unnamed" instead.

    if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"creating arg with argument = '%s', id = %d\n", argument, id);

    //Only allow a null string if specified
    if(!strcmp(argument, "")  && !allowNoString){
        if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"Tried to create command with no text but that is disallowed since allowNoString = %s\n", allowNoString?"True":"False");
        return arg_create_list(c, "unnammed", id, allowNoString);
    }
    //if the command with the current name already exists, append a cat num to it
    //char num[64] = {0};
    //num[0] = catNum%10 + '0';
    //if(command_id(argument, c) != ID_NONE) return arg_create_list(c, strcat(argument, num), id, allowNoString, catNum + 1);
	Arg *a = malloc(sizeof(Arg));
	//printf(LT_PRINT_WARN"creating NULL arglist\n");
    //free(a);
    //a = NULL;
    check_not_null(a, "new Arg list");
	strcpy(a->arg, argument);
	a->id = id;
	a->next = NULL;
    //if(catNum > 0) printf("Appended %d to %s due to an existing command\n",catNum, a->arg);
	return a;
}

Arg *append_arg(Commands *c, Arg *a, char argument[MAX_COMMAND_LENGTH], int id, int allowNoString){
	//appends an argument to an arg list

    if(lt_verbose == LT_VERBOSE_EXTRA){
        printf(LT_PRINT_INFO"Appending Arg with arg = '%s', and id = %d to arglist ",argument, id);
	    print_arg(a);
        printf("\n");
    }
    if(strcmp(argument, "") == 0 && !allowNoString){
        if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"Tried to append argument with no text but that is disallowed since allowNoString = %s\n",allowNoString?"True":"False");
        return a;
    }
    if(a == NULL) return arg_create_list(c, argument, id, allowNoString);
	Arg *head = a;
	while(a->next) a = a->next;
	a->next = arg_create_list(c, argument, id, allowNoString);
	//printf("\t");
	//print_arg(head);
	return head;
}

Arg *sanatise_command(char command[MAX_COMMAND_LENGTH], Arg *a, Commands *c){
	//returns a command given as a string in an arglist, that is each word seperated by ' ' character unless they are surrounded by '"' characters
    //removes any '"' characters

    //printf("sanatising the command %s\n",command);
	int j = 0;
	int in_quote = 0;
	char instr[MAX_COMMAND_LENGTH] = {0};
	while(j < MAX_COMMAND_LENGTH && command[j]  != '\0' && command[j] != '\n' && (command[j] != ' ' || in_quote == 1)){
		//adds first argument (the command)
        if(command[j] == '"'){
			in_quote = !in_quote;
			for(int c = j; c < MAX_COMMAND_LENGTH-1; c++){
				command[c] = command[c+1];
			}
			command[MAX_COMMAND_LENGTH - 1] = '\0';
			if(in_quote == 0){
				j --;
			}
		}
		instr[j] = command[j];
        //printf("instr = %s\n",instr);
		j ++;
	}
	//printf("%s\n",instr);
	a = arg_create_list(c, instr, command_id(instr,c), 0);
	in_quote = 0;
    int allowEmptyString = 0;
	while(j < MAX_COMMAND_LENGTH && command[j] != '\0'){
		//printf("\tup to %s\tj = %d\n",command + j,j);
		allowEmptyString = 0;

        char str[MAX_COMMAND_LENGTH] = {0};
		int i = 0;
		for(i = 0; j+i < MAX_COMMAND_LENGTH && (command[j+i] != ' ' || in_quote == 1) && command[j+i] != '\0' && command[j+i] != '\n'; i ++){
            while(command[j+i] == '"'){
                allowEmptyString = 1;
                //printf("command[%d] = %c, in_quote = %d\n",j+i, command[j+i], in_quote);
				in_quote = !in_quote;
				for(int c = j+i; c < MAX_COMMAND_LENGTH-1; c++){
					command[c] = command[c+1];
				}
				command[MAX_COMMAND_LENGTH-1] = '\0';
				if(in_quote == 0){
					i--;
				}
			}
			str[i] = command[j+i];
            //printf("j = %d, i = %d,in_quote = %d, command[%d] = %c\n",j,i,in_quote,j+i,command[j+i]);
            //printf("str = %s\n",str);
            //for(int c = 0; c < j + i; c ++) printf(" ");
            //printf("V\n");
            //printf("%s\n\n",command);
		}
		//printf("\tstr = %s\n",str);
		str[i] = '\0';
        //printf("trying to append '%s' to the arglist, isEmptyStirng = %d\n",str, allowEmptyString);
		a = append_arg(c, a, str, command_id(str,c), allowEmptyString);
		allowEmptyString = 0;
        j += i;
        //printf("j = %d\n",j);
		//printf("\tj = %d\n",j);
		if(i == 0){
			j ++;
            //printf("j incremented becuase i == 0\n");
		}
	}
	return a;
}


void print_help(Commands *c, int force){
	//prints all of the commands in a user friendly way, unless it is a hidden command
	//printf("EDITED\n");
    if(c == NULL) return;
	while(c){
		if(c->state == COM_SHOWN || force == PRINT_FORCE){
            printf(C_C"%s"C_W,c->command);
		    if(c->state == COM_HIDDEN) printf(" (hidden)");
            if(c->state == COM_SILENT) printf(" (silent)");
            printf("\n    %s\n",c->help_text);
        }
        c = c->next;
	}
}

Similar *new_similar(char *command){
    //creates a new similar node with the data of command
    //printf("Creating new similar entry with command: %s\n",command);
    Similar *s = malloc(sizeof(Similar));
    check_not_null(s, "new Similar list");
    strncpy(s->command, command, MAX_COMMAND_LENGTH);
    s->next = NULL;
    return s;
}

Similar *last_similar(Similar *s){
    //finds the last element of a similar list
    if(s == NULL) return s;
    while(s->next) s = s->next;
    return s;
}

Similar *append_similar(Similar *a, Similar *b){
    //appends a similar list to another
    if(a == NULL) return b;
    if(b == NULL) return a;
    last_similar(a)->next = b;
    return a;
}

void free_similar(Similar *s){
    //frees an entire similar list
    Similar *temp = s->next;
    while(s){
        temp = s->next;
        free(s);
        s = temp;
    }
    free(temp);
}

int diff_letters(char *str1, char *str2){
    //calculates how many different characters there are between two strings.
    //diff_letters("help","test") would return 3
    //diff_letters("help","helpext") would return 3 due to the three trailing characters in the second string
    int diff_count = 0;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int max = len1 * (len1 >= len2) + len2 * (len2 > len1);
    int min = len1 * (len1 < len2) + len2 * (len2 <= len1);
    for(int i = 0; i < min; i ++){
        diff_count += (str1[i] != str2[i]);
    }
    diff_count += max - min;
    return diff_count;
}

Similar *find_similar_commands(char *command, Commands *c, int sim){
    //finds commands with a maximum difference of sim characters from the command supplied
    Similar *s = NULL;
    while(c){
        int diff = diff_letters(command, c->command);
        //printf("%s has a similarity of %d  with %s\n",command, diff,  c->command);
        if(abs(diff) <= sim && c->state == COM_SHOWN){
            //the commands are similar enough (within sim characters difference)
            printf(LT_PRINT_INFO"Command '%s' is similar to '%s'\n",c->command, command);
            s = append_similar(s, new_similar(c->command));
        }
        c = c->next;
    }
    return s;
}

void print_similar(Similar *s){
    //prints all commands specified in a similar list
    if(s == NULL) return;
    while(s){
        printf(C_C"%s"C_W,s->command);
        if(s->next){
            printf(", ");
        }else{
            printf("\n");
        }
        s = s->next;
    }
}

int command_exists_str(Commands *c, char *command){
    //finds whether a command exists with a given string
    if(c == NULL) return 0;
    while(c){
        if(!strcmp(c->command, command) && c->state != COM_SILENT){
            if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_INFO"Command '%s' exists\n", command);
            return 1;
        }
        c = c->next;
    }
    if(lt_verbose == LT_VERBOSE_EXTRA) printf(LT_PRINT_WARN"Command '%s' does not exist\n", command);
    return 0;
}

int command_exists_id(Commands *c, int id){
    //finds if a command exists with a given id
    if(c == NULL) return 0;
    while(c){
        if(c->id == id && c->state != COM_SILENT){
            return 1;
        }
        c = c->next;
    }
    return 0;
}

int handle_input(Commands *c, Arg *a){
    //given a command list and argument list, prints an error if a non existant command was entered along with an similar commands
    //prints the help list if help is entered (and the help command exists)
    //exits the program if exit is entered (and the exit command exists)
    //otherwise if the command entered exists and is not hard coded, then print its response if it as one an return the id of the command executed
	int id = a->id;
	if(id == ID_NONE){
		Similar *s = find_similar_commands(a->arg, c, 3);
        if(command_exists_id(c, ID_HELP)){
            print_error("Unknown command. Enter "C_C"help"C_W" for a list of available commands.");
        }else{
            print_error("Unkown command.");
        }
        if(s){
            //if there are similar commands
            printf("Did you mean:\n");
            print_similar(s);
            free_similar(s);
        }
    }else{
		if(id == ID_HELP){
			lt_help_function(c, allowForcePrint);
			return id;
		}
		while(c && c->id != id) c = c->next;
		if(c->id == ID_EXIT && id == ID_EXIT){
			lt_exit_function();
		}
		if(c){
			if(c->response[0] != '\0' && c->response[0] != '\n'){
				printf("%s\n",c->response);
			}
		}else{
			print_error("Command ID not found in list of command IDs despite an ID being found originally. Please contact the developer of libtalaris");
			return -1;
		}
	}
	return id;
}

Arg *get_input(Commands *c, Arg *a/*, ArgStack *as*/){
    //reads a command from stdin
    //sanatises the command and turns it into an argument list
    //executes any hard coded commands and prints the response to a command if specified

	//printf("get_input called\n");
	char command[MAX_COMMAND_LENGTH] = {0};
	printf("%c ", PROMPT_CHAR);
    if(fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL){
        fprintf(stderr,"\nexiting due to EOF\n");
		exit(0);
	}
	a = sanatise_command(command,a,c);
	//printf("   command entered was '%s'\n",command);
	handle_input(c, a);
	//printf("   id was %d\n",id);
	return a;
}

void insert_at_end(char *first, char *second, int maxLen){
    //inserts a string at the end of another string, overwriting if the max length is exceeded
    //eg for insert_at_end("foo","bar",8) would change "foo" to "foobar"
    //however for insert_at_end("foo", "bar", 5) would change "foo" to "fobar", or (iae("foo","bar",4) would change "foo" to "fbar"

    //printf("insert at end called for '%s' with end insert = '%s', length1 = %lu, length2 = %lu\n",first, second, strlen(first), strlen(second));
    if((int)(strlen(first) + strlen(second)) > maxLen){
        if(strlen(second) > strlen(first)) return;
        for(int i = 0; i < (int)strlen(second); i ++){
            first[strlen(first) - strlen(second) + i] = second[i];
        }
    }else{
        strcat(first, second);
    }
    //printf("first = %s\n",first);
}

Commands *create_command_list(int id, char command[MAX_COMMAND_LENGTH], char response[MAX_RESPONSE_LENGTH], char help_text[MAX_HELP_TEXT_LENGTH], int state){
    //creates a new command list node with specifed data and returns it

    Commands *c = malloc(sizeof(Commands));
	check_not_null(c, "new Command list");
	strcpy(c->command, command);
	strcpy(c->response, response);
	strcpy(c->help_text, help_text);
	c->id = id;
    c->state = state;
	c->next = NULL;
	return c;
}

Arg *init_arg_list(){
    //initialises an arg, allocates memory and sets next to null

    Arg *a = malloc(sizeof(Arg));
	check_not_null(a, "initialising Arg list");
	a->next = NULL;
	return a;
}

Commands *init_command_list(){
    //creates the command list with hard coded commands for "help" and "exit"

	//printf("    init_command_list called\n");
	Commands *c = create_command_list(ID_HELP, "help", "", "Display this information.", COM_SHOWN);
	//printf("creating exit command\n");
	c->next = create_command_list(ID_EXIT, "exit", "", "Exits the program.", COM_SHOWN);
	return c;
}

int digit_num(int num){
    //returns the number of digits in a base 10 number

    double x = num;
    int i = 0;
    while(x >= 1){
        x = x/10.0;
        i ++;
    }
    return i;
}

Commands *append_command_list(Commands *c, int id, char command[MAX_COMMAND_LENGTH], char response[MAX_RESPONSE_LENGTH], char help_text[MAX_HELP_TEXT_LENGTH], int state){
    //appends a command to the command list with given data
    //if duplicate commands are now allowed (given by global variable allowDuplicateCommands) then if a command is given that has the same text as an existing one, it will append a number to the end of it eg "help2" if "help" exists

    if(c == NULL) return create_command_list(id, command, response, help_text, state);
	if(id == ID_EXIT || id == ID_HELP) return c;
	Commands *head = c;
    char nCommand[MAX_COMMAND_LENGTH] = {0};
    if(allowDuplicateCommands){
        //the command does not need to be unique, so copy as is
        strcpy(nCommand, command);
    }else{
        //make the command unique by appending a digit to its name if required
        strcpy(nCommand, command);
        int catNum = 2;
        while(command_id(nCommand, c) != ID_NONE && digit_num(catNum) < MAX_COMMAND_LENGTH){
            //loop to append digits until a unique version is found
            char num[64] = {0};
            int digits = digit_num(catNum);
            int temp = catNum;
            for(int i = digits-1; i >= 0; i --){
                num[i] = temp%10 + '0';
                temp -= (temp%10);
                temp /= 10;
            }
            strcpy(nCommand, command);
            insert_at_end(nCommand, num, MAX_COMMAND_LENGTH);
            catNum ++;
        }
    }
    //get to end of the list
	while(c->next != NULL) c = c->next;
    //append command
	c->next = create_command_list(id, nCommand, response, help_text, state);
	return head;
}

Commands *delete_command_char(Commands *c, char *command){
    //deletes a command with a given string by utilising the command_id function
    //if the command does not exist and the function is in verbose mode then print that

    int id = command_id(command, c);
    if(id == ID_NONE){
        if(lt_verbose) printf("command %s was not found\n",command);
        return c;
    }
    return delete_command(c, id);
}

Commands *delete_command(Commands *c, int id){
    //deletes a command with a given id, if none exists and verbose is on it will print that

    if(c == NULL){
        if(lt_verbose) printf("Error in delete_command: command list is empty\n");
        return NULL;
    }

    Commands *head = c;
	if(c->id == id){
        if(lt_verbose){
            printf("Deleting %s\n",c->command);
        }
		c = c->next;
		free(head);
		return c;
	}

    while(c && c->next){
		if(c->next->id == id){
            printf("Deleting %s\n",c->next->command);
			Commands *temp = c->next;
			c->next = c->next->next;
			free(temp);
			return head;
		}
		c = c->next;
	}
    if(lt_verbose) printf("No command with ID %d was found.\n",id);
	return head;
}

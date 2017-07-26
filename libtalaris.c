#include "libtalaris.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

void print_error(char *error){
    fprintf(stderr,C_R"ERROR"C_W": %s\n",error);
}

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

int command_id(char *command, Commands *c){
        if(c == NULL) return -1;
        while(c != NULL){
                if(strcmp(command, c->command) == 0){
                        return c->id;
                }
                c = c->next;
        }
        return -1;
}


Arg *find_element(Arg *a, int index){
	int i = 0;
	while(a && i < index){
		a = a->next;
		i++;
	}
	if(i != index) return NULL;
	return a;
}

//command stack

ArgStack *create_arg_stack(Arg *a){
    ArgStack *as = malloc(sizeof(ArgStack));
    assert(as);
    as->a = a;
    as->next = NULL;
    as->prev = NULL;
    return as;
}

ArgStack *last_arg_stack(ArgStack *as){
    if(as == NULL) return NULL;
    while(as->next) as = as->next;
    return as;
}

ArgStack *first_arg_stack(ArgStack *as){
    if(as == NULL) return NULL;
    while(as->prev) as = as->prev;
    return as;
}

ArgStack *push_arg_stack(ArgStack *a, ArgStack *b){
    if(a == NULL) return b;
    if(b == NULL) return a;
    a->prev = last_arg_stack(b);
    last_arg_stack(b)->next = a;
    return b;
}

//commands entered

void print_arg(Arg *a){
	printf("[");
	while(a){
		printf("%s",a->arg);
		if(a->next){
			printf(",");
		}
		a = a->next;
	}
	printf("]\n");
}

Arg *arg_create_list(char argument[MAX_COMMAND_LENGTH], int id){
	//printf("\t\tCreating Arg with arg %s, and id %d\n",argument, id);
	Arg *a = malloc(sizeof(Arg));
	assert(a);
	strcpy(a->arg, argument);
	a->id = id;
	a->next = NULL;
	return a;
}

Arg *append_arg(Arg *a, char argument[MAX_COMMAND_LENGTH], int id){
	//printf("\t\tAppending Arg with arg %s, and id %d\n",argument, id);
	if(strcmp(argument, "") == 0) return a;
	if(a == NULL) return arg_create_list(argument, id);
	Arg *head = a;
	while(a->next) a = a->next;
	a->next = arg_create_list(argument, id);
	//printf("\t");
	//print_arg(head);
	return head;
}

Arg *sanatise_command(char command[MAX_COMMAND_LENGTH], Arg *a, Commands *c){
	//printf("sanatising the command %s\n",command);
	int j = 0;
	int in_quote = 0;
	char instr[MAX_COMMAND_LENGTH] = {0};
	while(j < MAX_COMMAND_LENGTH && command[j]  != '\0' && command[j] != '\n' && (command[j] != ' ' || in_quote == 1)){
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
		j ++;
	}
	//printf("%s\n",instr);
	a = arg_create_list(instr, command_id(instr,c));
	in_quote = 0;
	while(j < MAX_COMMAND_LENGTH && command[j] != '\0'){
		//printf("\tup to %s\tj = %d\n",command + j,j);
		char str[MAX_COMMAND_LENGTH] = {0};
		int i = 0;
		for(i = 0; j+i < MAX_COMMAND_LENGTH && (command[j+i] != ' ' || in_quote == 1) && command[j+i] != '\0' && command[j+i] != '\n'; i ++){
			if(command[j+i] == '"'){
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
		}
		//printf("\tstr = %s\n",str);
		str[i] = '\0';
		a = append_arg(a, str, command_id(str,c));
		j += i;
		//printf("\tj = %d\n",j);
		if(i == 0){
			j ++;
		}
	}
	//printf("after sanatise_command, Arg list is:\n");
	//print_arg(a);
	return a;
}


void print_help(Commands *c){
	//printf("TESTING\n");
	if(c == NULL) return;
	while(c){
		printf(C_C"%s"C_W"\n    %s\n",c->command,c->help_text);
		c = c->next;
	}
}

Similar *new_similar(char *command){
    //printf("Creating new similar entry with command: %s\n",command);
    Similar *s = malloc(sizeof(Similar));
    assert(s);
    strncpy(s->command, command, MAX_COMMAND_LENGTH);
    s->next = NULL;
    return s;
}

Similar *last_similar(Similar *s){
    if(s == NULL) return s;
    while(s->next) s = s->next;
    return s;
}

Similar *append_similar(Similar *a, Similar *b){
    if(a == NULL) return b;
    if(b == NULL) return a;
    last_similar(a)->next = b;
    return a;
}

void free_similar(Similar *s){
    Similar *temp = s->next;
    while(s){
        temp = s->next;
        free(s);
        s = temp;
    }
    free(temp);
}

int diff_letters(char *str1, char *str2){
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
    Similar *s = NULL;
    while(c){
        int diff = diff_letters(command, c->command);
        //printf("%s has a similarity of %d  with %s\n",command, diff,  c->command);
        if(abs(diff) <= sim){
            //the commands are similar enough (within sim characters difference)
            s = append_similar(s, new_similar(c->command));
        }
        c = c->next;
    }
    return s;
}

void print_similar(Similar *s){
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

int handle_input(Commands *c, Arg *a){
	int id = a->id;
	if(id == -1){
		Similar *s = find_similar_commands(a->arg, c, 3);
        print_error("Unknown command. Enter "C_C"help"C_W" for a list of available commands.");
	    if(s){
            //if there are similar commands
            printf("Did you mean:\n");
            print_similar(s);
            free_similar(s);
        }
    }else{
		if(id == ID_HELP){
			print_help(c);
			return id;
		}
		while(c && c->id != id) c = c->next;
		if(c->id == ID_EXIT && id == ID_EXIT){
			printf("%s\n",c->response);
			exit(0);
		}
		if(c){
			if(c->response[0] != '\0' && c->response[0] != '\n'){
				printf("%s\n",c->response);
			}
		}else{
			print_error("Command ID not found in list of command IDs");
			return -1;
		}
	}
	return id;
}

Arg *get_input(Commands *c, Arg *a/*, ArgStack *as*/){
	//printf("get_input called\n");
	char command[MAX_COMMAND_LENGTH] = {0};
	printf("> ");
	if(fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL){
		printf("\nexiting due to EOF\n");
		exit(0);
	}
	a = sanatise_command(command,a,c);
	//printf("   command entered was '%s'\n",command);
	handle_input(c, a);
	//printf("   id was %d\n",id);
	return a;
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

Arg *init_arg_list(){
	Arg *a = malloc(sizeof(Arg));
	assert(a);
	a->next = NULL;
	return a;
}

Commands *init_command_list(){
	//printf("    init_command_list called\n");
	Commands *c = create_command_list(ID_HELP, "help", "", "Display this information.");
	//printf("creating exit command\n");
	c->next = create_command_list(ID_EXIT, "exit", "exiting program", "Exits the program.");
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

Commands *delete_command(Commands *c, int id){
	if(c == NULL) return NULL;
	Commands *head = c;
	if(c->id == id){
		c = c->next;
		free(head);
		return c;
	}
	while(c && c->next){
		if(c->next->id == id){
			Commands *temp = c->next;
			c->next = c->next->next;
			free(temp);
			return head;
		}
		c = c->next;
	}
	return c;
}

#include "libtalaris.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

//GLOBAL VARIBALES
int allowDuplicateCommands = 0;

int get_allowDuplicateCommands(void){
    return allowDuplicateCommands;
}

void set_allowDuplicateCommands(int x){
    allowDuplicateCommands = x;
}


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
	printf("]\n");
}

int command_id(char *command, Commands *c){
    if(c == NULL) return ID_NONE;
    if(strcmp(command, "") == 0) return ID_NONE;
    while(c != NULL){
        if(strcmp(command, c->command) == 0){
            return c->id;
        }
        c = c->next;
    }
    return ID_NONE;
}

char *command_str(int id, Commands *c){
    //TODO: Update this to be in the usual syntax with Commands *c first, as with above
    if(c == NULL) return "";
    while(c != NULL){
        if(c->id == id) return c->command;
        c = c->next;
    }
    return "";
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

Arg *arg_create_list(Commands *c, char argument[MAX_COMMAND_LENGTH], int id, int allowNoString){
	//printf("\t\tCreating Arg with arg %s, and id %d\n",argument, id);
    //Only allow a null string if specified
    if(!strcmp(argument, "")  && !allowNoString) return arg_create_list(c, "unnammed", id, allowNoString);
    //if the command with the current name already exists, append a cat num to it
    //char num[64] = {0};
    //num[0] = catNum%10 + '0';
    //if(command_id(argument, c) != ID_NONE) return arg_create_list(c, strcat(argument, num), id, allowNoString, catNum + 1);
	Arg *a = malloc(sizeof(Arg));
	assert(a);
	strcpy(a->arg, argument);
	a->id = id;
	a->next = NULL;
    //if(catNum > 0) printf("Appended %d to %s due to an existing command\n",catNum, a->arg);
	return a;
}

Arg *append_arg(Commands *c, Arg *a, char argument[MAX_COMMAND_LENGTH], int id, int allowNoString){
	//printf("\t\tAppending Arg with arg %s, and id %d\n",argument, id);
	if(strcmp(argument, "") == 0 && !allowNoString) return a;
	if(a == NULL) return arg_create_list(c, argument, id, allowNoString);
	Arg *head = a;
	while(a->next) a = a->next;
	a->next = arg_create_list(c, argument, id, allowNoString);
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

int command_exists_str(Commands *c, char *command){
    if(c == NULL) return 0;
    while(c){
        if(!strcmp(c->command, command)){
            return 1;
        }
        c = c->next;
    }
    return 0;
}

int command_exists_id(Commands *c, int id){
    if(c == NULL) return 0;
    while(c){
        if(c->id == id){
            return 1;
        }
        c = c->next;
    }
    return 0;
}

int handle_input(Commands *c, Arg *a){
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
			print_error("Command ID not found in list of command IDs despite an ID being found originally. Please contact the developer of libtalaris");
			return -1;
		}
	}
	return id;
}

Arg *get_input(Commands *c, Arg *a/*, ArgStack *as*/){
	//printf("get_input called\n");
	char command[MAX_COMMAND_LENGTH] = {0};
	printf("%c ", PROMPT_CHAR);
    if(fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL){
        //printf("\nexiting due to EOF\n");
		exit(0);
	}
	a = sanatise_command(command,a,c);
	//printf("   command entered was '%s'\n",command);
	handle_input(c, a);
	//printf("   id was %d\n",id);
	return a;
}

void insert_at_end(char *first, char *second, int maxLen){
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

int digit_num(int num){
    double x = num;
    int i = 0;
    while(x >= 1){
        x = x/10.0;
        i ++;
    }
    return i;
}

Commands *append_command_list(Commands *c, int id, char command[MAX_COMMAND_LENGTH], char response[MAX_RESPONSE_LENGTH], char help_text[MAX_HELP_TEXT_LENGTH]){
	if(c == NULL) return create_command_list(id, command, response, help_text);
	if(id == ID_EXIT || id == ID_HELP) return c;
	Commands *head = c;
    char nCommand[MAX_COMMAND_LENGTH] = {0};
    if(allowDuplicateCommands){
        strcpy(nCommand, command);
    }else{
        //printf("copying %s into nCommand\n",command);
        strcpy(nCommand, command);
        //printf("nCommand = %s\n",nCommand);
        int catNum = 2;
        while(command_id(nCommand, c) != ID_NONE){
            //printf("\n");
            char num[64] = {0};
            int digits = digit_num(catNum);
            //printf("%d has %d digits\n",catNum, digits);
            int temp = catNum;
            for(int i = digits-1; i >= 0; i --){
                num[i] = temp%10 + '0';
                //printf("temp = %d, temp mod 10 = %d\n",temp,temp%10);
                //printf("num[%d] = %c\n",i,num[i]);
                temp -= (temp%10);
                temp /= 10;
                //printf("temp = %d\n\n",temp);
            }
            //printf("num = %s\n",num);
            strcpy(nCommand, command);
            //printf("nCommand = %s\n",nCommand);
            insert_at_end(nCommand, num, MAX_COMMAND_LENGTH);
            //printf("nCommand = %s\n",nCommand);
            catNum ++;
        }
    }
    //printf("nCommand = %s\n",nCommand);
	while(c->next != NULL){
		c = c->next;
	}
	c->next = create_command_list(id, nCommand, response, help_text);
	//print_list_commands(head);
	return head;
}

Commands *delete_command_char(Commands *c, char *command, int verbose){
    int id = command_id(command, c);
    if(id == ID_NONE) return c;
    return delete_command(c, id, verbose);
}

Commands *delete_command(Commands *c, int id, int verbose){
	if(c == NULL) return NULL;

    Commands *head = c;
	if(c->id == id){
        if(verbose){
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
    printf("No command with ID %d was found.\n",id);
	return head;
}

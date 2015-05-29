#include "PossibleMove.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#define BLACK 0
#define WHITE 1
#define MAX_ERROR_MSG 0x1000

char** board;
int player;
int AI;
int maxRecursionDepth;


static int compile_regex(regex_t* r, const char* regex_text){
    int status = regcomp (r, regex_text, REG_EXTENDED);
	if (status == 0){
		return 0;
	}
    char error_message[MAX_ERROR_MSG];
	regerror(status, r, error_message, MAX_ERROR_MSG);
    printf("Regex error compiling '%s': %s\n", regex_text, error_message);
    return 1;
}

void initialize(){
	board = Board_new();
	Board_init(board);
	player = WHITE;
	AI     = BLACK;
	maxRecursionDepth = 1;
}

int executeCommand(char* command){
	command = strtok(command, "\n");
	if (strcmp(command, "clear") == 0){
		Board_clear(board);
		return 0;
	}
	if (strcmp(command, "print") == 0){
		Board_print(board);
		return 0;
	}
	return 1;
}

int settings(){
	printf("Enter game settings:\n");
	char command[256];
	while (1){
		if (fgets(command, 256, stdin) == NULL){
			printf("error reading command\n");
			continue;
		}
		if (strcmp(command, "quit\n") == 0){
			return 0;
		}
		if (strcmp(command, "start\n") == 0){
			return 1;
		}
		int exitcode = executeCommand(command);
	}
}

void game(){
	
}

int main(){
	initialize();
	printf("Welcome to Draughts!\n");
	int start = settings();
	if (start){
		game();
	}
	free(board);
	return 0;
}


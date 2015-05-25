#include "Board.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#define BLACK 0
#define WHITE 1

char board[Board_SIZE][Board_SIZE];
int player;
int AI;
int maxRecursionDepth;


void initialize(){
	Board_init(board);
	player = WHITE;
	AI     = BLACK;
	maxRecursionDepth = 1;
}

int readCommand(char* command){
	return 0;
}

int settings(){
	printf("Enter game settings:\n");
	char command[256];
	while (1){
		if (fgets(command, 256, stdin) == NULL){
			printf("error reading command\n");
			continue;
		}
		if (strcmp(command, "clear\n") == 0){
			Board_clear(board);
			continue;
		}
		if (strcmp(command, "print\n") == 0){
			Board_print(board);
			continue;
		}
		if (strcmp(command, "quit\n") == 0){
			return 0;
		}
		if (strcmp(command, "start\n") == 0){
			return 1;
		}
		int exitcode = readCommand(command);
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
	return 0;
}


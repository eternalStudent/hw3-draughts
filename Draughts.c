#include "PossibleMove.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#define BLACK 0
#define WHITE 1
#define MAX_ERROR_MSG 0x1000
#define SETTINGS 0
#define GAME     1
#define MOVE    17

char** board;
int player;
int AI;
int maxRecursionDepth;
int state;


static int compile_regex(regex_t* r, const char* regex_text){
    int status = regcomp(r, regex_text, REG_EXTENDED);
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
	state = SETTINGS;
}

//TODO: add regex functions

int executeCommand(char* command){
	command = strtok(command, "\n");
	if (state = SETTINGS){
		if (strcmp(command, "clear") == 0){
			Board_clear(board);
			return 0;
		}
		if (strcmp(command, "print") == 0){
			Board_print(board);
			return 0;
		}
		if (strcmp(command, "start") == 0){
			if (Board_isPlayable(board)){
				state = GAME;
				return 0;
			}
			return -1;
		}
		//TODO complete
	}
	else{
		//TODO complete
	}
	return 1;
}

void printError(int error){
	//TODO complete
}

char** minimax(char** board, int depth, int color){
	if (depth == 0){
		return board;
	}
	struct LinkedList* possibleMoves = Board_getPossibleMoves(board, color);
	struct PossibleMove* bestPossibleMove;
	int extremum = 101;
	struct Iterator* iterator = Iterator_new(possibleMoves);
	while (Iterator_hasNext(iterator)) {
		struct PossibleMove* currentPossibleMove = (struct PossibleMove*)Iterator_next(iterator); 
		int score = Board_getScore( minimax(currentPossibleMove->board, !color, depth-1) ); 
		if (player == BLACK){
			score = -score;
		}
		if (extremum == 101 || 
				(color == AI     && score >  extremum) || 
				(color == player && score <  extremum) || 
				(rand()%2        && score == extremum)
			){
			extremum = score;
			bestPossibleMove = currentPossibleMove;
		}
	}
	return bestPossibleMove->board;
}

void computerTurn(){
	char** bestBoard = minimax(board, maxRecursionDepth, AI);
	Board_copy(board, bestBoard);
}

int main(){
	initialize();
	printf("Welcome to Draughts!\n");
	printf("Enter game settings:\n");
	char command[256];
	while (1){
		if (fgets(command, 256, stdin) == NULL){
			perror("Error: standard function fgets has failed\n");
			return -1;
		}
		if (strcmp(command, "quit\n") == 0){
			break;
		}
		int exitcode = executeCommand(command);
		if (exitcode){
			printError(exitcode);
			continue;
		}
		int commandWasMove = (exitcode == MOVE);
		if (state == GAME && commandWasMove){
			Board_print(board);
			int playerWon = (Board_getScore(board) == 100);
			if (playerWon){
				printf("%s player wins!/n", (player == BLACK)? "Black": "White");
				break;
			}
			computerTurn();
			Board_print(board);
			int computerWon = (Board_getScore(board) == -100);
			if (playerWon){
				printf("%s player wins!/n", (AI == BLACK)? "Black": "White");
				break;
			}
		}
	}
	free(board);
	return 0;
}
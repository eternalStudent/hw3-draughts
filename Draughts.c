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

char* getSubstring(char* str, int i, int range){
	char* substring = calloc(range+1, sizeof(char));
	strncpy(substring, str+i, range);
	return substring;
}

void initialize(){
	board = Board_new();
	Board_init(board);
	player = WHITE;
	AI     = BLACK;
	maxRecursionDepth = 1;
	state = SETTINGS;
}

//TODO: add game state regex functions

/* 
 * Various regex functions for executing the correct command as input by the user.
 *
 * @params: the input command string
 
 * @return: 1 if the command didn't match, 0 if the command matched and was executed successfully, the appropriate error code otherwise 
 */ 

int setMinimaxDepth (char* str){
	regex_t r; 	
	regmatch_t matches[2];
	int exitcode = 1; /*did not match*/
	char* pattern = "^minimax_depth\\s+(-?[0-9]+)$";
	compile_regex(&r, pattern);
	if (regexec(&r, str, 2, matches, 0) == 0){
		int depth;
		char* depthAsString;
		char* ptr;
		while(1){
			int start = matches[1].rm_so;
			int range = matches[1].rm_eo-start;
			depthAsString = getSubstring(str, start, range);
			depth = strtol(depthAsString, &ptr, 10);
			if(depth < 1 || depth > 6){
				exitcode = 11; /* Error: illegal minimax depth */
				break;
			}
			maxRecursionDepth = depth;
			exitcode = 0; /* Recursion depth set successfully */
			break;
		}
		free(depthAsString);
	}
	regfree(&r);
	return exitcode;
}

int setUserColor (char* str){
	regex_t r; 	
	regmatch_t matches[2];
	int exitcode = 1; /*did not match*/
	char* pattern = "^user_color\\s+(black|white)\\s*$";
	compile_regex(&r, pattern);
	if (regexec(&r, str, 2, matches, 0) == 0){
		char* color;
		while(1){
			int start = matches[1].rm_so;
			int range = matches[1].rm_eo-start;
			color = getSubstring(str, start, range);
			if(strcmp(color, "black") == 0){
				player = BLACK;
				exitcode = 0; /* Player's colour set successfully */
				break;
			}
			else if(strcmp(color, "white") == 0){
				player = WHITE;
				exitcode = 0; /* Player's colour set successfully */
				break;
			}		
		}
		free(color);
	}
	regfree(&r);
	return exitcode;
}

int removePiece(char* str){
	regex_t r; 	
	regmatch_t matches[3];
	int exitcode = 1; /*did not match*/
	char* pattern = "^rm\\s+<([a-z]),([0-9]+)>\\s*$";
	compile_regex(&r, pattern);
	if (regexec(&r, str, 3, matches, 0) == 0){
		char* columnAsString;
		char column;
		char* rowAsString;
		int row;
		char* ptr;	
		while(1){
			int start = matches[1].rm_so;
			int range = matches[1].rm_eo-start;
			columnAsString = getSubstring(str, start, range);
			column = *columnAsString;
			int columnAsInt = (int)column-96;
			
			start = matches[2].rm_so;
			range = matches[2].rm_eo-start;
			rowAsString = getSubstring(str, start, range);
			row = strtol(rowAsString, &ptr, 10);
			
			if((row < 0) || (row > Board_SIZE) || (columnAsInt < 0) || (columnAsInt > Board_SIZE) || ((row + columnAsInt) % 2 != 0)){
				exitcode = 12; /* invalid position on the board */
				break;
			}
			Board_remove(board, column, row);
			exitcode = 0;
			break;
		}
		free(columnAsString);
		free(rowAsString);
	}
	regfree(&r);
	return exitcode;
}

int setPiece(char* str){
	regex_t r; 	
	regmatch_t matches[5];
	int exitcode = 1; /*did not match*/
	char* pattern = "^set\\s+<([a-z]),([0-9]+)>\\s*(white|black)\\s*(m|k)\\s*$";
	compile_regex(&r, pattern);
	if (regexec(&r, str, 5, matches, 0) == 0){
		char* columnAsString;
		char column;
		char* rowAsString;
		int row;
		char* ptr;
		char piece;
		char* pieceColor;
		char* pieceRank;
		while(1){
			int start = matches[1].rm_so;
			int range = matches[1].rm_eo-start;
			columnAsString = getSubstring(str, start, range);
			column = *columnAsString;
			int columnAsInt = (int)column-96;
			
			start = matches[2].rm_so;
			range = matches[2].rm_eo-start;
			rowAsString = getSubstring(str, start, range);
			row = strtol(rowAsString, &ptr, 10);
			
			start = matches[3].rm_so;
			range = matches[3].rm_eo-start;
			pieceColor = getSubstring(str, start, range); 
			
			start = matches[4].rm_so;
			range = matches[4].rm_eo-start;
			pieceRank = getSubstring(str, start, range);
			
			
			if(strcmp(pieceColor,"white") == 0){
				if(strcmp(pieceRank,"m") == 0){
					piece = Board_WHITE_MAN;
				}
				else{
					piece = Board_WHITE_KING;
				}
			}
			else{
				if(strcmp(pieceRank,"m") == 0){
					piece = Board_BLACK_MAN;
				}
				else{
					piece = Board_BLACK_KING;
				}
			}		
			
			if((row < 0) || (row > Board_SIZE) || (columnAsInt < 0) || (columnAsInt > Board_SIZE) || ((row + columnAsInt) % 2 != 0)){
				exitcode = 12; /* invalid position on the board */
				break;
			}	
			Board_set(board, column, row, piece);
			exitcode = 0; /* piece set successfully */
			break;
		}
		free(columnAsString);
		free(rowAsString);
		free(pieceColor);
		free(pieceRank);
	}
	regfree(&r);
	return exitcode;
}

int executeCommand(char* command){
	int error;
	command = strtok(command, "\n");
	if (state == SETTINGS){
		if (strcmp(command, "clear") == 0){
			Board_clear(board);
			return 0;
		}
		if (strcmp(command, "print") == 0){
			printf("matched print\n");
			Board_print(board);
			return 0;
		}
		if (strcmp(command, "start") == 0){
			if (Board_isPlayable(board)){
				state = GAME;
				return 0;
			}
			return 13;
		}		
		error = setMinimaxDepth(command);
		if(error != 1){
			return error;
		}	
		error = setUserColor(command);
		if(error != 1){
			return error;
		}
		error = removePiece(command);
		if(error != 1){
			return error;
		}
		error = setPiece(command);
		if(error != 1){
			return error;
		}
	}
	else{
	//TODO: complete game state functions
	}
	return -2;
}

void printError(int error){
	if (error == -2){
		printf("Illegal command, please try again\n");
	}
	if (error == 11){
		printf("Wrong value for minimax depth. The value should be between 1 to 6\n");
	}
	if (error == 12){
		printf("Invalid position on the board\n");
	}
	if (error == 13){
		printf("Wrong board initialization\n");
	}
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
			fprintf(stderr, "Error: standard function fgets has failed\n");
			break;
		}
		if (strcmp(command, "quit\n") == 0){
			break;
		}
		if (strcmp(command, "\n") == 0){
			continue;
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
				printf("%s player wins!\n", (player == BLACK)? "Black": "White");
				break;
			}
			computerTurn();
			Board_print(board);
			int computerWon = (Board_getScore(board) == -100);
			if (playerWon){
				printf("%s player wins!\n", (AI == BLACK)? "Black": "White");
				break;
			}
		}
	}
	Board_free(board);
	return 0;
}
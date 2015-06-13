#include "PossibleMoveList.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#define MAX_ERROR_MSG 0x1000
#define SETTINGS 0
#define GAME     1
#define MOVE    17

char** board;
int player;
int AI;
int maxRecursionDepth;
int state;
struct LinkedList* playerPossibleMoves;
struct LinkedList* computerPossibleMoves;

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

int allocationFailed(void* ptr){
	if (ptr == NULL){
		fprintf(stderr, "Error: standard function calloc has failed\n");
		return 1;
	}
	return 0;
}

void initialize(){
	board = Board_new();
	if (allocationFailed(board)){
		exit(0);
	}
	Board_init(board);
	player = WHITE;
	AI     = BLACK;
	maxRecursionDepth = 1;
	state = SETTINGS;
	playerPossibleMoves = NULL;
	computerPossibleMoves = NULL;
}

/* 
 * Sets the minimax depth according to input from the user.
 *
 * @params: the input command string
 * @return: 1 if the command didn't match, 
 *          0 if the command matched and was executed successfully, 
 *          11 if the user input illegal minimax depth
 */ 
int setMinimaxDepth (char* str){
	regex_t r; 	
	regmatch_t matches[2];
	char* pattern = "^minimax_depth\\s+(-?[0-9]+)$";
	int depth;
	compile_regex(&r, pattern);
	if (regexec(&r, str, 2, matches, 0) != 0){
		regfree(&r);
		return 1;
	}
	int start = matches[1].rm_so;
	depth = strtol(str+start, NULL, 10);
	if(depth < 1 || depth > 6){
		regfree(&r);
		return 11;
	}	
	maxRecursionDepth = depth;
	return 0;
}

/* 
 * Sets the user's color according to input from the user.
 *
 * @params: the input command string*
 * @return: 1 if the command didn't match, 0 if the command matched and was executed successfully
 */ 
int setUserColor (char* str){
	regex_t r; 	
	regmatch_t matches[2];
	char* pattern = "^user_color\\s+((black)|(white))\\s*$";
	compile_regex(&r, pattern);
	if (regexec(&r, str, 2, matches, 0) != 0){
		regfree(&r);
		return 1;
	}
	
	int start = matches[1].rm_so;
	if(str[start] == 'b'){
		player = BLACK;
		regfree(&r);
		return 0;
	}
	player = WHITE;
	regfree(&r);
	return 0;
}

void getPosition(char* str, regmatch_t matches[], int* x, int* y){
	int start = matches[1].rm_so;
	char column = str[start];
	*x = (int)column-96;
	
	start = matches[2].rm_so;
	*y = strtol(str+start, NULL, 10);
}

/* 
 * Removes a piece currently on the board according to input from the user.
 *
 * @params: the input command string
 * @return: 01 if the command didn't match, 
 *          00 if the command matched and was executed successfully, 
 *          12 if the user input an illegal position on the board 
 */ 
int removePiece(char* str){
	regex_t r; 	
	regmatch_t matches[3];
	char* pattern = "^rm\\s+<([a-z]),([0-9]+)>\\s*$";
	compile_regex(&r, pattern);
	if (regexec(&r, str, 3, matches, 0) != 0){
		regfree(&r);
		return 1;
	}
	
	int x, y;
	getPosition(str, matches, &x, &y);
	if(!Board_isValidPosition(board, x, y) || Board_isEmpty(board, x, y)){
		regfree(&r);
		return 12; 
	}
	board[x-1][y-1] = Board_EMPTY;
	regfree(&r);
	return 0;
}

char getPiece(char* str, regmatch_t matches[]){
	char piece;
	char color = str[matches[3].rm_so];
	char rank  = str[matches[6].rm_so];
	
	if(color == 'w'){
		if(rank == 'm'){
			piece = Board_WHITE_MAN;
		}
		else{
			piece = Board_WHITE_KING;
		}
	}
	else{
		if(rank == 'm'){
			piece = Board_BLACK_MAN;
		}
		else{
			piece = Board_BLACK_KING;
		}
	}
	return piece;
}

/* 
 * Places a piece on the board according to input from the user.
 *
 * @params: the input command string
 * @return: 01 if the command didn't match, 
 *          00 if the command matched and was executed successfully, 
 *          12 if the user input an illegal position on the board
 */ 
int setPiece(char* str){
	regex_t r; 	
	regmatch_t matches[7];
	char* pattern = "^set\\s+<([a-z]),([0-9]+)>\\s*((white)|(black))\\s*(m|k)\\s*$";
	compile_regex(&r, pattern);
	if (regexec(&r, str, 7, matches, 0) != 0){
		regfree(&r);
		return 1;
	}
	
	int x, y;
	getPosition(str, matches, &x, &y);
	if (!Board_isValidPosition(board, x, y) || !Board_isEmpty(board, x, y)){
		regfree(&r);
		return 12;
	}
	
	char piece = getPiece(str, matches);
	board[x-1][y-1] = piece;
	regfree(&r);
	return 0;
}

/* 
 * Performs a move on the board according to input from the user.
 * The move can consist of a single step, or several steps.
 *
 * @params: the input command string
 * @return: 01 if the command didn't match,  
 *          12 if the user input an illegal position on the board,
 *          14 if the initial tile doesn't contain one of the player's pieces, 
 *          15 if the move itself is illegal, 
 *          17 if the move was carried out successfully
 *          21 if any allocation errors occurred
 */ 
int movePiece(char* str){
	regex_t r; 	
	regmatch_t matches[5];
	int exitcode;
	char* pattern = "^move\\s+<([a-z]),([0-9]+)>\\s+to\\s*((<[a-z],[0-9]+>)+)";
	compile_regex(&r, pattern);
	struct PossibleMove* possibleMove = NULL;
	while(1){
		if (regexec(&r, str, 5, matches, 0) != 0){
			exitcode = 1;
			break;
		}	
		
		//starting position
		int x, y;
		getPosition(str, matches, &x, &y);
		if (!Board_isValidPosition(board, x, y)){
			exitcode = 12;
			break;
		}
		if (!Board_isPieceInSpecifiedColor(board, x, y, player)){
			exitcode = 14;
			break;
		}
		
		//destination positions
		struct LinkedList* moves = LinkedList_new(&Tile_free);
		if (allocationFailed(moves)){
			exitcode = 21;
			break;
		}
			
		char* destPosSection = (char*)calloc(strlen(str+matches[3].rm_so)+1, sizeof(char));
		if (allocationFailed(destPosSection)){
			exitcode = 21;
			break;
		}
		
		strcpy(destPosSection,str+matches[3].rm_so);
		char* token;
		token = strtok(destPosSection, "><");
		while (token != NULL) {
			int x = (int)token[0]-96;
			int y = strtol(token+2,NULL,10);
			if (!Board_isValidPosition(board, x, y)){
				LinkedList_free(moves);
				free(destPosSection);
				regfree(&r);
				return 12; // must return with number instead of setting exitcode and breaking,	because breakpoint is inside two loops
			}	
			struct Tile* nextMove = Tile_new(x,y);
			LinkedList_add(moves, nextMove);
			token = strtok(NULL, "><");
		}	
		free(destPosSection);
		
		//constructing the move structure
		possibleMove = PossibleMove_new(x, y, moves, board);
		if (allocationFailed(possibleMove)){
			exitcode = 21;
			break;
		}
		//making sure move is legal
		if (!PossibleMoveList_contains(playerPossibleMoves, possibleMove)){
			exitcode = 15;
			break;
		}
		//if all preconditions are met, the move is carried out
		Board_update(board, possibleMove);
		exitcode = 17;
		break;
	}
	regfree(&r);
	if (possibleMove != NULL){
		PossibleMove_free(possibleMove);
	}
	return exitcode;
}

int updatePossibleMoves(){
	if (playerPossibleMoves){
		LinkedList_free(playerPossibleMoves);
	}
	playerPossibleMoves = Board_getPossibleMoves(board, player);
	if (allocationFailed(playerPossibleMoves)){
		return 21;
	}
	
	if (computerPossibleMoves){
		LinkedList_free(computerPossibleMoves);
	}
	computerPossibleMoves = Board_getPossibleMoves(board, AI);
	if (allocationFailed(computerPossibleMoves)){
		return 21;
	}
	
	return 0;
}

void freeGlobals(){
	Board_free(board);
	if (playerPossibleMoves){
		LinkedList_free(playerPossibleMoves);
	}
	if (computerPossibleMoves){
		LinkedList_free(computerPossibleMoves);
	}
}

int executeCommand(char* command){
	int error;
	command = strtok(command, "\n");
	if (state == SETTINGS){
		if (strcmp(command, "quit") == 0){
			freeGlobals();
			exit(0);
		}
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
				return updatePossibleMoves();
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
		if (strcmp(command, "get_moves") == 0){
			PossibleMoveList_print(playerPossibleMoves);
			return 0;
		}
		
		if (strcmp(command, "quit") == 0){
			freeGlobals();
			exit(0);
		}
		
		error = movePiece(command);
		if(error != 1){
			return error;
		}
	}
	return -2;
}

void printError(int error){
	switch(error){
		case (0):
			break;
		case(-2):
			printf("Illegal command, please try again\n");
			break;
		case(11):
			printf("Wrong value for minimax depth. The value should be between 1 to 6\n");
			break;
		case(12):
			printf("Invalid position on the board\n");
			break;
		case(13):
			printf("Wrong board initialization\n");
			break;
		case(14):
			printf("The specified position does not contain your piece\n");
			break;
		case(15):
			printf("Illegal move\n");
			break;
		case(17):
			break;
		case(21):
			freeGlobals();
			exit(0);
		default:
			printf("Illegal command, please try again\n");
			break;
	}
}

/*
 * Evaluates the board according to the specified scoring function.
 *
 * @return: a numeric evaluation of the board
 */
int Board_getScore(char** board){
	if ((LinkedList_length(playerPossibleMoves) == 0)){ // no possible moves left for human player, computer wins
			return 100;
		}
		
	if ((LinkedList_length(computerPossibleMoves) == 0)){ // no possible moves left for computer, human player wins
			return -100;
		}

	int score = 0;
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){		
			char piece = board[x][y];
			if (piece == Board_BLACK_MAN){
				score--;
				continue;
			}
			if (piece == Board_WHITE_MAN){
				score++;
				continue;
			}
			if (piece == Board_BLACK_KING){
				score-=3;
				continue;
			}
			if (piece == Board_WHITE_KING){
				score+=3;
				continue;
			}
		}
	}
	
	return score;
}

char** minimax(char** board, int depth, int color){
	if (depth == 0){
		return board;
	}
	struct LinkedList* possibleMoves = Board_getPossibleMoves(board, color);
	struct PossibleMove* bestPossibleMove;
	int extremum = 101;
	struct Iterator iterator;
	Iterator_init(&iterator, possibleMoves);
	while (Iterator_hasNext(&iterator)) {
		struct PossibleMove* currentPossibleMove = (struct PossibleMove*)Iterator_next(&iterator);
		int score = Board_getScore( minimax(currentPossibleMove->board, !color, depth-1) ); 
		if (player == BLACK && (abs(score) != 100)) { // if score is 100 or (-100) it doesn't depend on the players' colours,
            score = -score;                           // only on the fact that one of them no longer has possible moves. Therefore, it shouldn't be inverted.*/
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
	printError(updatePossibleMoves());
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
		if (strcmp(command, "\n") == 0){
			continue;
		}
		int exitcode = executeCommand(command);
		printError(exitcode);
		if (state == GAME && exitcode == MOVE){
			Board_print(board);
			int playerWon = (Board_getScore(board) == -100);
			if (playerWon){
				printf("%s player wins!\n", (player == BLACK)? "Black": "White");
				break;
			}
			computerTurn();
			Board_print(board);
			int computerWon = (Board_getScore(board) == 100);
			if (computerWon){
				printf("%s player wins!\n", (AI == BLACK)? "Black": "White");
				break;
			}
		}
	}
	freeGlobals();
	return 0;
}
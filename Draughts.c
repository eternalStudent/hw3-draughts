#include "PossibleMoveList.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#define MAX_ERROR_MSG 0x1000
#define SETTINGS 0
#define GAME     1
#define UNDEFINED 101

char** board;
int human;
int maxRecursionDepth;
int state;
struct LinkedList* humanPossibleMoves;
int turn;

/*
 * Compiles regular expression.
 *
 * @params: (r) - a pointer to a regex type
 *          (regex_text) - the regex to be compiled
 * @return: 1 if the expression is not a regex, 0 otherwise
 */
int compile_regex(regex_t* r, const char* regex_text){
    int status = regcomp(r, regex_text, REG_EXTENDED);
	if (status == 0){
		return 0;
	}
    char error_message[MAX_ERROR_MSG];
	regerror(status, r, error_message, MAX_ERROR_MSG);
    printf("Regex error compiling '%s': %s\n", regex_text, error_message);
    return 1;
}

/*
 * Checks whether an allocation has failed
 * 
 * @params: (ptr) - a pointer to the data that was allocated
 * @return: true (1) if the allocation has failed, false (0) otherwise 
 */
int allocationFailed(void* ptr){
	if (ptr == NULL){
		fprintf(stderr, "Error: standard function calloc has failed\n");
		return 1;
	}
	return 0;
}


/*
 * Initializes the global variables.
 */
void initialize(){
	board = Board_new();
	if (allocationFailed(board)){
		exit(0);
	}
	Board_init(board);
	human = WHITE;
	maxRecursionDepth = 1;
	state = SETTINGS;
	humanPossibleMoves = NULL;
	turn = human;
}

/*
 * Frees the allocated global variables.
 */
void freeGlobals(){
	Board_free(board);
	if (humanPossibleMoves != NULL){
		LinkedList_free(humanPossibleMoves);
	}
}

/*
 * Frees the allocated global variables and exit the program.
 */
void freeAndExit(){
	freeGlobals();
	exit(0);
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
	regfree(&r);
	return 0;
}

/* 
 * Sets the user's color according to input from the user.
 *
 * @params: str - the command
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
	human = (str[start] == 'b')? BLACK: WHITE;
	turn = human;
	regfree(&r);
	return 0;
}

/*
 * Parse the position of a tile from a regmatch type array.
 *
 * @params: str       - the string to be parsed
 *          matches[] - the regmatch type array from which to parse the tile.
 *          (x, y)    - pointers to the variables to which the position will be parsed
 */
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
	Board_removePiece(board, x, y);
	regfree(&r);
	return 0;
}

/*
 * Parse a piece type from a regmatch type array.
 *
 * @params: str       - the string to be parsed
 *          matches[] - the regmatch type array from which to parse the tile.
 * @return: a char representing the parsed piece
 */
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
	Board_setPiece(board, x, y, piece);
	regfree(&r);
	return 0;
}

int populateSteps(struct LinkedList* steps, char* str){
	char* token = strtok(str, "><");
	while (token != NULL) {
		int x = (int)token[0]-96;
		int y = strtol(token+2,NULL,10);
		if (!Board_isValidPosition(board, x, y)){
			LinkedList_free(steps);
			return 1;
		}	
		LinkedList_add(steps, Tile_new(x,y));
		token = strtok(NULL, "><");
	}	
	return 0;
}

/* 
 * Performs a move on the board according to input from the user.
 * The move can consist of a single step, or several steps.
 *
 * @params: the input command string
 * @return: 00 if the move was carried out successfully
 *          01 if the command didn't match,  
 *          12 if the user input an illegal position on the board,
 *          14 if the initial tile doesn't contain one of the player's pieces, 
 *          15 if the move itself is illegal, 
 *          21 if any allocation errors occurred
 */ 
int movePiece(char* str){
	regex_t r; 	
	regmatch_t matches[5];
	int exitcode;
	char* pattern = "^move\\s+<([a-z]),([0-9]+)>\\s+to\\s*((<[a-z],[0-9]+>)+)";
	compile_regex(&r, pattern);
	struct PossibleMove* move = NULL;
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
		if (Board_evalPiece(board, x, y, human) <= 0){
			exitcode = 14;
			break;
		}
		
		//destination positions
		struct LinkedList* steps = LinkedList_new(&Tile_free);
		if (allocationFailed(steps)){
			exitcode = 21;
			break;
		}
			
		if (populateSteps(steps, str+matches[3].rm_so)){
			exitcode = 12;
			break;
		}
		
		//constructing the move structure
		move = PossibleMove_new(x, y, steps, board);
		if (allocationFailed(move)){
			exitcode = 21;
			break;
		}
		//making sure move is legal
		if (!PossibleMoveList_contains(humanPossibleMoves, move)){
			exitcode = 15;
			break;
		}
		//if all preconditions are met, the move is carried out
		Board_update(board, move);
		LinkedList_free(humanPossibleMoves);
		humanPossibleMoves = NULL;
		exitcode = 0;
		turn = !turn;
		Board_print(board);
		break;
	}
	regfree(&r);
	if (move != NULL){
		PossibleMove_free(move);
	}
	return exitcode;
}

/*
 * Updates the global variable (humanPossibleMoves).
 *
 * @return: 21 if any allocation errors occurred, 0 otherwise
 */
int updatePossibleMoves(){
	if (humanPossibleMoves){
		LinkedList_free(humanPossibleMoves);
		humanPossibleMoves = NULL;
	}
	
	humanPossibleMoves = Board_getPossibleMoves(board, human);
	if (allocationFailed(humanPossibleMoves)){
		return 21;
	}
	return 0;
}


/*
 * Populates a command string to a pointer read from the user.
 *
 * @params: (command) - the string to be populated
 */
void readCommand(char command[]){
	if (fgets(command, 256, stdin) == NULL){
		fprintf(stderr, "Error: standard function fgets has failed\n");
		freeAndExit();
	}
}

/*
 * Executes a command given by the user
 *
 * @params: (command) - the command given by the user
 * @return: relevant exitcode
 */
int executeCommand(char* command){
	int error;
	command = strtok(command, "\n");
	if (state == SETTINGS){
		if (strcmp(command, "quit") == 0){
			freeAndExit();
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
				turn = WHITE;
				if (human == WHITE){
					printf("Enter your move:\n");
				}
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
			PossibleMoveList_print(humanPossibleMoves);
			return 0;
		}
		
		if (strcmp(command, "quit") == 0){
			freeAndExit();
		}
		
		error = movePiece(command);
		if(error != 1){
			return error;
		}
	}
	return -2;
}

/*
 * Prints relevant error message.
 *
 * @params: (error) - the exitcode of the error
 */
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
			freeAndExit();
		default:
			printf("Illegal command, please try again\n");
			break;
	}
}

/*
 * The minimax AI algorithm.
 */
struct PossibleMove* minimax(struct PossibleMove* possibleMove, int depth, int player){
	if (depth == 0){
		return possibleMove;
	}
	char** board = possibleMove->board;
	struct LinkedList* possibleMoves = Board_getPossibleMoves(board, player);
	if (LinkedList_length(possibleMoves) == 0){
		LinkedList_free(possibleMoves);
		return possibleMove;
	}
	if (LinkedList_length(possibleMoves) == 1){
		struct PossibleMove* onlyMove = PossibleMoveList_first(possibleMoves);
		LinkedList_freeAllButOne(possibleMoves, onlyMove);
		return onlyMove;
	}
	
	struct PossibleMove* bestPossibleMove;
	int extremum = UNDEFINED;
	struct Iterator iterator;
	Iterator_init(&iterator, possibleMoves);
	while (Iterator_hasNext(&iterator)) {
		struct PossibleMove* currentPossibleMove = (struct PossibleMove*)Iterator_next(&iterator);
		struct PossibleMove* temp = minimax(currentPossibleMove, depth-1, player);
		int score = Board_getScore(temp->board, player);
		if (currentPossibleMove != temp){
			PossibleMove_free(temp);
		}		
		if (extremum == UNDEFINED || 
				(player != human && score >  extremum) || 
				(player == human && score <  extremum) || 
				(rand()%2       && score == extremum)
			){
			extremum = score;
			bestPossibleMove = currentPossibleMove;
		}
	}
	LinkedList_freeAllButOne(possibleMoves, bestPossibleMove);
	return bestPossibleMove;
}

/*
 * The computer turn procedure.
 */
void computerTurn(){
	struct PossibleMove possibleMove;
	possibleMove.board = board;
	struct PossibleMove* bestMove = minimax(&possibleMove, maxRecursionDepth, !human);
	printf("Computer: ");
	PossibleMove_print(bestMove);
	printf("\n");
	Board_update(board, bestMove);
	PossibleMove_free(bestMove);
	printError(updatePossibleMoves());
	turn = !turn;
	Board_print(board);
}

/*
 * The human turn procedure
 */
void humanTurn(){
	if (state == GAME){
		printf("Enter your move:\n");
	}
	
	while (turn == human){
		char command[256];
		readCommand(command);
		int error = executeCommand(command);
		printError(error);
	}
}

int main(){
	initialize();
	printf("Welcome to Draughts!\n");
	printf("Enter game settings:\n");
	int gameOver = 0;
	while (!gameOver){
		if (turn == human){
			humanTurn();
		}
		else{
			computerTurn();
		}
		
		gameOver = (Board_getScore(board, turn) == -100);
	}
	printf("%s player wins!\n", (turn == BLACK)? "White" : "Black");
	freeGlobals();
	return 0;
}
#include <stdio.h>
#include "Board.h"

char** Board_new(){
	char** board = calloc(Board_SIZE, sizeof(char*));
	if (!board){
		return NULL;
	}
	for(int i = 0; i < Board_SIZE; i++){
		board[i] = calloc(Board_SIZE, sizeof(char));
	}
	return board;
}

/*
 * Populates the board in the standard way.
 */
void Board_init(char** board){
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			if ((x + y) % 2 == 0){
				if (y <= 3){
					board[x][y] = Board_WHITE_MAN;
					continue;
				}
				if (y >= 6){
					board[x][y] = Board_BLACK_MAN;
					continue;
				}
			}
			board[x][y] = Board_EMPTY;
		}
	}
}

/*
 * Clears the board from all pieces.
 */
void Board_clear(char** board){
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			board[x][y] = Board_EMPTY;
		}
	}
}

/*
 * Populates a board according to another board.
 *
 * @params: dest - a pointer to the board to be populated, 
 *          src  - a pointer to the board according to whom dest will be populated
 */
void Board_copy(char** dest, char** src){
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			dest[x][y] = src[x][y];
		}
	}
}

static int charToInt(char ch){
	return ((int)ch)-97;
}

int isInRange(int x, int y){
	if (x >=0 && x<Board_SIZE && y>=0 && y<Board_SIZE){
		return 1;
	}
	return 0;
}

static int isOnBlack(int x, int y){
	return (x + y) % 2 == 0;
}

/*
 * Place a piece on the board.
 *
 * @params: (x, y) - the coordinates of the tile, 
 *          piece  - the piece to be put
 * @return: -1 if (x, y) is out of range or on a white tile, 0 otherwise
 */
int Board_set(char** board, char ch, int y, char piece){
	int x = charToInt(ch);
	if (isInRange(x, y) && isOnBlack(x, y)){
		board[x][y] = piece;
		return 0;
	}
	return -1;
}

/*
 * Removes a piece from the board.
 * 
 * @params: (x, y) - the coordinates of the tile in which the piece to be removed is placed
 * @return: -1 if the coordinates are out of range, 0 otherwise
 */
int Board_remove(char** board, char ch, int y){
	int x = charToInt(ch);
	if (isInRange(x, y)){
		board[x][y] = Board_EMPTY;
		return 0;
	}
	return -1;
}

int Board_isPlayable(char** board){
	int countBlack = 0;
	int countWhite = 0;
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			if (board[x][y] == Board_BLACK_KING || board[x][y] == Board_BLACK_MAN){
				countBlack++;
				continue;
			}
			if (board[x][y] == Board_WHITE_KING || board[x][y] == Board_WHITE_MAN){
				countWhite++;
				continue;
			}
		}
	}
	int tooFew  = (countBlack == 0 || countWhite == 0);
	int tooMany = (countBlack > 20 || countWhite > 20);
	return (!tooFew && !tooMany);
}

/*
 * Moves a piece to a different tile in the board.
 *
 * @params: (oldX, oldY) - the coordinates of the piece to be moved
            (newX, newY) - the coordinates the piece will be moved to
 * @return: -1 if any of the coordinates are out of range or the desired location
 *          is occupied or the desired location is a white tile, 0 otherwise
 */
static void Board_move(char** board, char oldCh, int oldY, char newCh, int newY){
	int oldX = charToInt(oldCh);
	int newX = charToInt(newCh);
	char piece = board[oldX][oldY];
	board[oldX][oldY] = Board_EMPTY;
	board[newX][newY] = piece;
	//TODO complete
}

char** Board_getPossibleBoard(char** board, struct PossibleMove* move){
	char** possibleBoard = Board_new();
	Board_copy(possibleBoard, board);
	//TODO complete
	return possibleBoard;
}

struct LinkedList* Board_getPossibleMoves(char** board, int player){
	struct LinkedList* list = LinkedList_new(&PossibleMove_free);
	//TODO complete
	return list;
}

/*
 * Evaluates the board according to the specified scoring function.
 *
 * @return: a numeric evaluation of the board
 */
int Board_getScore(char** board){
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

static void printLine(){
	printf("  |");
	for (int x = 1; x < Board_SIZE*4; x++){
		printf("-");
	}
	printf("|\n");
}

/*
 * Prints an ASCII representation of the board.
 */
void Board_print(char** board){
	printLine();
	for (int y = Board_SIZE-1; y >= 0 ; y--){
		printf((y < 9? " %d": "%d"), y+1);
		for (int x = 0; x < Board_SIZE; x++){
			printf("| %c ", board[x][y]);
		}
		printf("|\n");
		printLine();
	}
	printf("   ");
	for (int y = 0; y < Board_SIZE; y++){
		printf(" %c  ", (char)('a' + y));
	}
	printf("\n");
}

void Board_free(char** board){
	for(int i = 0; i < Board_SIZE; i++){
		free(board[i]);
	}
	free(board);
}
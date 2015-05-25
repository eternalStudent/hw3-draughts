#include <stdio.h>
#include "Board.h"

/*
 *  Populates the board in the standard way.
 */
void Board_init(char[Board_SIZE][Board_SIZE] board){
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
void Board_clear(char[Board_SIZE][Board_SIZE] board){
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			board[x][y] = Board_EMPTY;
		}
	}
}

static int charToInt(char ch){
	return ((int)ch)-97;
}

static int isInRange(int x, int y){
	if (x >=0 && x<Board_SIZE && y>=0 && y<Board_SIZE){
		return 1;
	}
	return 0;
}

/*
 * Place a piece on the board.
 *
 * @params: (x, y) are the coordinates of the tile, piece is the piece to be put.
 * @return: -1 if (x, y) is out of range, 0 otherwise;
 */
int Board_set(char[Board_SIZE][Board_SIZE] board, char ch, int y, char piece){
	int x = charToInt(ch);
	if (isInRange(x, y)){
		board[x][y] = piece;
		return 0;
	}
	return -1;
}

/*
 * Removes a piece from the board.
 * 
 * @params: (x, y) are the coordinates of the tile in which the piece to be removed is placed.
 * @return: -1 if the coordinates are out of range, 0 otherwise.
 */
int Board_remove(char[Board_SIZE][Board_SIZE] board, char ch, int y){
	int x = charToInt(ch);
	if (isInRange(x, y)){
		board[x][y] = Board_EMPTY;
		return 0;
	}
	return -1;
}

/*
 * Moves a piece to a different tile in the board.
 *
 * @params: (oldX, oldY) are the coordinates of the piece to be moved. (newX, newY) are the coordinates the piece will be moved to.
 * @return: -1 if any of the coordinates are out of range or the desired location is occupied, 0 otherwise.
 */
int Board_move(char[Board_SIZE][Board_SIZE], char oldCh, int oldY, char newCh, int newY){
	int oldX = charToInt(oldCh);
	int newX = charToInt(newCh);
	if (isInRange(oldX, oldY) && isInRange(newX, newY) && board[newX][newY] == Board_EMPTY){
		char piece = board[oldX][oldY];
		board[oldX][oldY] = Board_EMPTY;
		board[newX][newY] = piece;
		return 0;
	}
	return -1;
}

/*
 * Evaluates the board.
 *
 * @return: a numeric evaluation of the board.
 */
int Board_getScore(char[Board_SIZE][Board_SIZE] board){
	int score = 0;
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			char piece = board[x][y];
			if (piece == Board_BLACK_MAN){
				score++;
				continue;
			}
			if (piece == Board_WHITE_MAN){
				score--;
				continue;
			}
			if (piece == Board_BLACK_KING){
				score+=3;
				continue;
			}
			if (piece == Board_WHITE_KING){
				score-=3;
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
void Board_print(char[Board_SIZE][Board_SIZE] board){
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
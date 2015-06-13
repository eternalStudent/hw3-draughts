#include <stdio.h>
#include "Board.h"
#define BLACK 0
#define WHITE 1


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

int charToInt(char ch){
	return ((int)ch)-96;
}

static int isInRange(int x, int y){
	if (x >= 1 && x <= Board_SIZE && y >= 1 && y <= Board_SIZE){
		return 1;
	}
	return 0;
}

static int isOnBlack(int x, int y){
	return !((x+y)%2);
}

int Board_isEmpty(char** board, int x, int y){
	return board[x-1][y-1] == Board_EMPTY;
}

int Board_isValidPosition(char** board, int x, int y){
	return (isInRange(x,y) && isOnBlack(x,y));
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

static void Board_crownPieces(char** board){
	for (int x = 0; x<Board_SIZE; x++){
		if (board[x][0] == Board_BLACK_MAN) {
			board[x][0] = Board_BLACK_KING;
		}
		if (board[x][Board_SIZE-1] == Board_WHITE_MAN){
			board[x][Board_SIZE-1] = Board_WHITE_KING;
		}
	}	
}

static void Board_removeCaptured(char** board, int oldX, int oldY, int newX, int newY){
	if (abs(newX-oldX) > 1 && abs(newY-oldY) > 1){
		int currX = oldX;
		int currY = oldY;
		while ((currX != newX) && (currY != newY)){
			board[currX-1][currY-1] = Board_EMPTY;
			if(newX-oldX>0){
				currX++;
			}
			else{
				currX--;
			}
			if(newY-oldY>0){
				currY++;
			}
			else{
				currY--;
			}
		}
	}
}

/*
 * Moves a piece to a different tile in the board.
 *
 * @params: (oldX, oldY) - the coordinates of the piece to be moved
 *          (newX, newY) - the coordinates the piece will be moved to
 */
static void Board_move(char** board, char oldCh, int oldY, char newCh, int newY){
	int oldX = charToInt(oldCh);
	int newX = charToInt(newCh);
	char piece = board[oldX-1][oldY-1];
	board[oldX-1][oldY-1] = Board_EMPTY;
	board[newX-1][newY-1] = piece;
	
	Board_crownPieces(board);
	Board_removeCaptured(board, oldX, oldY, newX, newY);
}

void Board_update(char** board, struct PossibleMove* move){
	struct Tile* current = move->start;
	struct LinkedList* moves = move->moves;
	struct Iterator iterator;
	Iterator_init(&iterator, moves);
	while(Iterator_hasNext(&iterator)){
		struct Tile* dest = (struct Tile*)Iterator_next(&iterator);
		Board_move(board, current->x, current->y, dest->x, dest->y);
		current = dest;
	}
}

/*
 * Creates a board representing the state of the board after a possible move has been carried out.
 *
 * @params: move - a pointer to the move to be carried out.
 * @return: NULL if any allocation errors occurred, the new board otherwise
 */
char** Board_getPossibleBoard(char** board, struct PossibleMove* move){
	char** possibleBoard = Board_new();
	if (!possibleBoard){
		return NULL;
	}
	Board_copy(possibleBoard, board);
	Board_update(possibleBoard, move);
	return possibleBoard;
}

int Board_isPieceInSpecifiedColor(char** board, int x, int y, int color){
	char playerColor[2];
	if (color == BLACK){
		char playerColor[2] = {Board_BLACK_MAN, Board_BLACK_KING};
	}	
	else{
		char playerColor[2] = {Board_WHITE_MAN, Board_WHITE_KING}; 
	}
	return (board[x-1][y-1] == playerColor[0] || board[x-1][y-1] == playerColor[1]);
}

static struct LinkedList* getPossibleJumps (char** currentBoard, int player){
	struct LinkedList* jumpMovesList = LinkedList_new(&PossibleMove_free);
	
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			if (!Board_isPieceInSpecifiedColor(currentBoard, x+1, y+1, player)){
				continue;
			}
			for (int i = -1; i <= 1; i += 2){
				for (int j = -1; j<= 1; j +=2){
					if (!isInRange(x+i+1,y+j+1) || !isInRange(x+2*i+1,y+2*j+1)){
						continue;
					}
					int enemyNearby = Board_isPieceInSpecifiedColor(currentBoard, x+i+1, y+i+1, !player);
					int enemyIsCapturable = (currentBoard[x+2*i][y+2*j] == Board_EMPTY);
					if(enemyNearby && enemyIsCapturable){
						struct Tile* destTile = Tile_new(x+2*i+1, y+2*j+1);
						struct LinkedList* individualJumpMovesList = LinkedList_new(&Tile_free);
						LinkedList_add(individualJumpMovesList, destTile);
						struct PossibleMove* newJumpMove = PossibleMove_new(x+1, y+1, individualJumpMovesList, currentBoard);
						LinkedList_add(jumpMovesList, newJumpMove);
					}
				}
			}
		}	
	}
	return jumpMovesList;
}

static struct LinkedList* getPossibleSingleMoves (char** currentBoard, int player){
	struct LinkedList* singleMovesList = LinkedList_new(&PossibleMove_free);
	int forward = (player == BLACK) ? -1 : 1; /* for each player's different direction of "forward" */
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			if (!Board_isPieceInSpecifiedColor(currentBoard, x+1, y+1, player)){
				continue;
			} 
			for (int i = -1; i <= 1; i += 2){
				if (!isInRange(x+i+1, y+forward+1)){
					continue;
				}
				if(currentBoard[x+i][y+forward] == Board_EMPTY){
					struct Tile* destTile = Tile_new(x+i+1, y+forward+1);
					struct LinkedList* singleMoveTileList = LinkedList_new(&Tile_free);
					LinkedList_add(singleMoveTileList, destTile);
					struct PossibleMove* newSingleMove = PossibleMove_new(x+1, y+1, singleMoveTileList, currentBoard);
					LinkedList_add(singleMovesList, newSingleMove);
				}
			}
		}
	}
	return singleMovesList;
}

struct LinkedList* Board_getPossibleMoves(char** board, int player){
	struct LinkedList* jumpMovesList = getPossibleJumps(board, player);
	if (LinkedList_length(jumpMovesList) != 0){ /* if jumps are possible, they are the only type of move possible */
		return jumpMovesList;
	}
	struct LinkedList* singleMovesList = getPossibleSingleMoves(board, player);
	return singleMovesList;
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
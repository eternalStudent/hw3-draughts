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
	return ((int)ch)-96;
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

static int isValidPosition(int x, int y, char** board){
	return (isInRange(x,y) && isOnBlack(x,y) && board[x][y] == Board_EMPTY);
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
		board[x-1][y-1] = piece;
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
		board[x-1][y-1] = Board_EMPTY;
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
 *          (newX, newY) - the coordinates the piece will be moved to
 * @return: -1 if any of the coordinates are out of range or the desired location
 *          is occupied or the desired location is a white tile, 0 otherwise
 */
static int Board_move(char** board, char oldCh, int oldY, char newCh, int newY){
	int oldX = charToInt(oldCh);
	int newX = charToInt(newCh);
	if (!isValidPosition(oldX,oldY,board) || !isValidPosition(newX,newY,board)){
		return -1;
	}	
	char piece = board[oldX][oldY];
	board[oldX][oldY] = Board_EMPTY;
	board[newX][newY] = piece;
	
	/* When a MAN reaches the furthest row from where it started it becomes a KING */
	if (piece == Board_BLACK_MAN && newY == 1) {
		board[newX][newY] = Board_BLACK_KING;
	}
	if (piece == Board_WHITE_MAN && newY == Board_SIZE){
		board[newX][newY] = Board_WHITE_KING;
	}

	/* If the move is a jump, all jumped pieces must be removed from the board*/
	if (abs(newX-oldX)>1 && abs(newY-oldY)>1){
		int tempX = oldX;
		int tempY = oldY;
		
		while ((tempX != newX) && (tempY != newY)){
			board[tempX][tempY] = Board_EMPTY;
			if(newX-oldX>0){
				tempX++;
			}
			else{
				tempX--;
			}
			if(newY-oldY>0){
				tempY++;
			}
			else{
				tempY--;
			}
		}
	}
	return 0;
}

/*
 * Creates a board representing the state of the board after a possible move has been carried out.
 *
 * @params: (board) is the the existing state of the board and (move) is a specific set of moves currently possible 
 * @return: NULL if any of the moves specified is illegal or impossible, the possible state of the board following this move otherwise
 */
char** Board_getPossibleBoard(char** board, struct PossibleMove* move){
	int error;
	char** possibleBoard = Board_new();
	Board_copy(possibleBoard, board);
	struct Tile* startingTile = move->start;
	struct LinkedList* moves = move->moves;
	struct Iterator* iterator = Iterator_new(moves);
	while(Iterator_hasNext(iterator)){
		if(iterator->current == NULL){
			Iterator_next(iterator);
			error = Board_move(possibleBoard, startingTile->x, startingTile->y, ((struct Tile*)(iterator->current->data))->x, ((struct Tile*)(iterator->current->data))->y);
			continue;
		}
		else {
			Iterator_next(iterator);
			if(Iterator_hasNext(iterator)){
			error = Board_move(possibleBoard, ((struct Tile*)(iterator->current->data))->x, ((struct Tile*)(iterator->current->data))->y,
			        ((struct Tile*)(iterator->current->next->data))->x, ((struct Tile*)(iterator->current->next->data))->y);
			}
		}
		if (error != 0){
			return NULL;
		}
	}
	Iterator_free(iterator);
	return possibleBoard;
}

struct LinkedList* Board_getPossibleMoves(char** board, int player){
	struct LinkedList* jumpMovesList = getPossibleJumps(board, player);
	if (jumpMovesList -> first != NULL){ /* if jumps are possible, they are the only type of move possible */
		return jumpMovesList;
	}
	struct LinkedList* singleMovesList = getPossibleSingleMoves(board, player);
	return singleMovesList;
}

struct LinkedList* getPossibleJumps (char** currentBoard, int player){
	struct LinkedList* jumpMovesList = LinkedList_new(&PossibleMove_free);
	char playerColor[2];
	char opponentColor[2];
	
	if (player == 0){
		char playerColor[2] = {Board_BLACK_MAN,Board_BLACK_KING};
		char opponentColor[2] = {Board_WHITE_MAN,Board_WHITE_KING};
	}
	else{
		char playerColor[2] = {Board_WHITE_MAN,Board_WHITE_KING}; 
		char opponentColor[2] = {Board_BLACK_MAN,Board_BLACK_KING}; 
	}
	
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			if (currentBoard[x][y] == playerColor[0]){ /* player has a MAN in this location*/
				struct Tile* startingTile = Tile_new((char)(x+97), y+1);
				
				
				if(isInRange(x-1,y-1) && (currentBoard[x-1][y-1] == opponentColor[0] || currentBoard[x-1][y-1] == opponentColor[1])){
					if (isInRange(x-2,y-2) && currentBoard[x-2][y-2] == Board_EMPTY){
						struct Tile* destTile = Tile_new((char)(x+95), y-1);
						struct LinkedList* individualJumpMovesList = LinkedList_new(&Tile_free);
						LinkedList_add(individualJumpMovesList, destTile);
						struct PossibleMove* newJumpMove = PossibleMove_new(startingTile, individualJumpMovesList, currentBoard);
						LinkedList_add(jumpMovesList, newJumpMove);
					}
				}
				
				if(isInRange(x-1,y+1) && (currentBoard[x-1][y+1] == opponentColor[0] || currentBoard[x-1][y+1] == opponentColor[1])){
					if (isInRange(x-2,y+2) && currentBoard[x-2][y+2] == Board_EMPTY){
						struct Tile* destTile = Tile_new((char)(x+95), y+3);
						struct LinkedList* individualJumpMovesList = LinkedList_new(&Tile_free);
						LinkedList_add(individualJumpMovesList, destTile);
						struct PossibleMove* newJumpMove = PossibleMove_new(startingTile, individualJumpMovesList, currentBoard);
						LinkedList_add(jumpMovesList, newJumpMove);
					}
				}
				
				if(isInRange(x+1,y-1) && (currentBoard[x+1][y-1] == opponentColor[0] || currentBoard[x+1][y-1] == opponentColor[1])){
					if (isInRange(x+2,y-2) && currentBoard[x+2][y-2] == Board_EMPTY){
						struct Tile* destTile = Tile_new((char)(x+99), y-1);
						struct LinkedList* individualJumpMovesList = LinkedList_new(&Tile_free);
						LinkedList_add(individualJumpMovesList, destTile);
						struct PossibleMove* newJumpMove = PossibleMove_new(startingTile, individualJumpMovesList, currentBoard);
						LinkedList_add(jumpMovesList, newJumpMove);
					}
				}
				
				if(isInRange(x+1,y+1) && (currentBoard[x+1][y+1] == opponentColor[0] || currentBoard[x+1][y+1] == opponentColor[1])){
					if (isInRange(x+2,y+2) && currentBoard[x+2][y+2] == Board_EMPTY){
						struct Tile* destTile = Tile_new((char)(x+99), y+3);
						struct LinkedList* individualJumpMovesList = LinkedList_new(&Tile_free);
						LinkedList_add(individualJumpMovesList, destTile);
						struct PossibleMove* newJumpMove = PossibleMove_new(startingTile, individualJumpMovesList, currentBoard);
						LinkedList_add(jumpMovesList, newJumpMove);
					}
				}	
			}
	
			if (currentBoard[x][y] == playerColor[1]){ /* player has a KING in this location */
				
			}
		}
	}
	return jumpMovesList;
}

struct LinkedList* getPossibleSingleMoves (char** currentBoard, int player){
	struct LinkedList* singleMovesList = LinkedList_new(&PossibleMove_free);
	char playerColor[2];
	if (player == 0){
		char playerColor[2] = {Board_BLACK_MAN,Board_BLACK_KING};
	}
	else{
		char playerColor[2] = {Board_WHITE_MAN,Board_WHITE_KING}; 
	}
	int adjustment = (player == 0) ? 1 : (-1); /* for each player's different direction of "forward" */
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			if (currentBoard[x][y] == playerColor[0] || currentBoard[x][y] == playerColor[1]){ /* player has a piece in this location*/
					if (isInRange(x-adjustment,y-adjustment) && currentBoard[x-adjustment][y-adjustment] == Board_EMPTY){ /* move right & forward is possible */
						struct Tile* startingTile = Tile_new((char)x+97,y+1);
						struct Tile* destinationTile = Tile_new((char)(x+97-adjustment),y-adjustment+1);
						struct LinkedList* followingTiles = LinkedList_new(&Tile_free);
						LinkedList_add(followingTiles, destinationTile);
						struct PossibleMove* move = PossibleMove_new(startingTile, followingTiles, currentBoard);
						LinkedList_add(singleMovesList, move);
					}
					
					if (isInRange(x+adjustment,y-adjustment) && currentBoard[x+adjustment][y-adjustment] == Board_EMPTY){ /* move left & forward is possible */
						struct Tile* startingTile = Tile_new((char)x+97,y+1);
						struct Tile* destinationTile = Tile_new((char)(x+97+adjustment),y-adjustment+1);
						struct LinkedList* followingTiles = LinkedList_new(&Tile_free);
						LinkedList_add(followingTiles, destinationTile);
						struct PossibleMove* move = PossibleMove_new(startingTile, followingTiles, currentBoard);
						LinkedList_add(singleMovesList, move);
					} 					
				}
			}
		}
		return singleMovesList;
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
	
	//TODO: factor in every player's possibility to make further moves (or lack thereof)
	
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
#include <stdio.h>
#include "Board.h"
#define BLACK 0
#define WHITE 1

/*
 * Creates a new board structure.
 *
 * @return: the new board structured as a two-dimensional char array.
 */
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

void Board_setPiece(char** board, int x, int y, char piece){
	board[x-1][y-1] = piece;
}

char Board_getPiece(char** board, int x, int y){
	return board[x-1][y-1];
}

char Board_removePiece(char** board, int x, int y){
	char piece = Board_getPiece(board, x, y);
	board[x-1][y-1] = Board_EMPTY;
	return piece;
}

/*
 * Checks if the input coordinates are within the range of the board's length and width.
 *
 * @params: (x, y) - the coordinates to be checked
 * @return: 1 if the coordinates correspond to a valid position on the board, 0 otherwise
 */
static int isInRange(int x, int y){
	if (x >= 1 && x <= Board_SIZE && y >= 1 && y <= Board_SIZE){
		return 1;
	}
	return 0;
}

/*
 * Checks if the input coordinates correspond to a black tile on the board.
 *
 * @params: (x, y) - the coordinates to be checked
 * @return: 1 if the coordinates correspond to a black tile on the board, 0 otherwise
 */
static int isOnBlack(int x, int y){
	return !((x+y)%2);
}

/*
 * Checks if the input coordinates correspond to an empty tile on the board.
 *
 * @params: (x, y) - the coordinates to be checked
 *			(board) - the board to be checked
 * @return: 1 if the coordinates correspond to an empty tile on the board, 0 otherwise
 */
int Board_isEmpty(char** board, int x, int y){
	return board[x-1][y-1] == Board_EMPTY;
}

/*
 * Checks if the input coordinates correspond to a valid tile position on the board.
 * That is, a position that is both within range and black.
 *
 * @params: (x, y) - the coordinates to be checked
 *			(board) - the board to be checked
 * @return: 1 if the coordinates correspond to a valid position on the board, 0 otherwise
 */
int Board_isValidPosition(char** board, int x, int y){
	return (isInRange(x,y) && isOnBlack(x,y));
}

/*
 * Checks if the input board is playable. Specifically, checks that the board is not empty,
 * has pieces of both colors, and that no color has over 20 pieces.  
 *
 * @params: (board) - the board to be checked		
 * @return: 1 if the board is playable, 0 otherwise
 */
int Board_isPlayable(char** board){
	int countBlack = 0;
	int countWhite = 0;
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			char piece = Board_getPiece(board, x, y);
			if (piece == Board_BLACK_KING || piece == Board_BLACK_MAN){
				countBlack++;
				continue;
			}
			if (piece == Board_WHITE_KING || piece == Board_WHITE_MAN){
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
 * Scans the top and bottom rows of the board, and "crowns" the appropriate MAN pieces.  
 *
 * @params: (board) - the board to be checked		
 */
static void Board_crownPieces(char** board){
	for (int x = 1; x <= Board_SIZE; x++){
		if (Board_getPiece(board, x, 1) == Board_BLACK_MAN) {
			Board_setPiece(board, x, 1, Board_BLACK_KING);
		}
		if (Board_getPiece(board, x, Board_SIZE) == Board_WHITE_MAN){
			Board_setPiece(board, x, Board_SIZE, Board_WHITE_KING);
		}
	}	
}

/*
 * Removes the captured piece as part of a jump move.  
 *
 * @params: (board) - the board on which the move is played
 *			(oldX, oldY) - the coordinates of the piece to be moved
 *			(newX, newY) - the coordinates the piece will be moved to
 */
static void Board_removeCaptured(char** board, int oldX, int oldY, int newX, int newY){
	if (abs(newX-oldX) > 1 && abs(newY-oldY) > 1){
		int currX = oldX;
		int currY = oldY;
		while ((currX != newX) && (currY != newY)){
			Board_removePiece(board, currX, currY);
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
	Board_removePiece(board, oldX, oldY);
	Board_setPiece(board, newX, newY, piece);
	
	Board_crownPieces(board);
	Board_removeCaptured(board, oldX, oldY, newX, newY);
}

/*
 * Updates a board according to a possible move.
 *
 * @params: (board) - the board to be updated
 *          (move) - the move to be carried out on the board 
 */
void Board_update(char** board, struct PossibleMove* move){
	struct Tile* current = move->start;
	struct LinkedList* steps = move->steps;
	struct Iterator iterator;
	Iterator_init(&iterator, steps);
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
char** Board_getPossibleBoard(char** board, struct PossibleMove* possibleMove){
	char** possibleBoard = Board_new();
	if (!possibleBoard){
		return NULL;
	}
	Board_copy(possibleBoard, board);
	Board_update(possibleBoard, possibleMove);
	return possibleBoard;
}

/*
 * Evaluates a single piece on the board according to the provided scoring function. 
 *
 * @params: (board) - the board that hosts the evaluated piece
 *          (x,y) - the coordinates of the piece to be evaluated
 *			(color) - the color of the player the scoring function is adjusted for.
 */
int Board_evalPiece(char** board, int x, int y, int player){
	char piece = Board_getPiece(board, x, y);
	int value = 0;
	if (piece == Board_WHITE_MAN){
		value = 1;
	}
	if (piece == Board_WHITE_KING){
		value = 3;
	}
	if (piece == Board_BLACK_MAN){
		value = -1;
	}
	if (piece == Board_BLACK_KING){
		value = -3;
	}
	if (player == BLACK){
		return -value;
	}
	return value;
}

/*
 * Checks if a piece currently has any possible single step moves.
 *
 * @params: (board) - the board to be checked
 *          (x,y) - the coordinates of the piece to be checked
 *			(player) - the player the check is done for
 * @return: 1 if the piece has a possible single step move, 0 otherwise
 */
int isSingleStepPossible(char** board, int x, int y, int player){
	int forward = (player == BLACK)? -1 : 1;
	if (Board_evalPiece(board, x, y, player) <= 0){
		return 0;
	} 
	for (int i = -1; i <= 1; i += 2){
		if (!isInRange(x+i, y+forward)){
			continue;
		}
		if(Board_isEmpty(board, x+i, y+forward)){
			return 1;
		}
	}
	return 0;
}

/*
 * Checks if a piece currently has any possible jump moves.
 *
 * @params: (board) - the board to be checked
 *          (x,y) - the coordinates of the piece to be checked
 *			(player) - the player the check is done for
 * @return: 1 if the piece has a possible jump move, 0 otherwise
 */
int isJumpPossible(char** board, int x, int y, int player){
	if (Board_evalPiece(board, x, y, player) <= 0){
		return 0;
	}
	for (int i = -1; i <= 1; i += 2){
		for (int j = -1; j <= 1; j += 2){
			if (!isInRange(x+i,y+j) || !isInRange(x+2*i,y+2*j)){
				continue;
			}
			int enemyNearby = Board_evalPiece(board, x+i, y+j, !player) > 0;
			int enemyIsCapturable = Board_isEmpty(board, x+2*i, y+2*j);
			if(enemyNearby && enemyIsCapturable){
				return 1;
			}
		}
	}
	return 0;
}

/*
 * Evaluates the board according to the specified scoring function.
 *
 * @return: a numeric evaluation of the board
 */
int Board_getScore(char** board, int player){
	int score = 0;
	int hasMoves = 0;
	int opponentHasMoves = 0;
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			int value = Board_evalPiece(board, x, y, player);
			hasMoves += (value > 0) && (isSingleStepPossible(board, x, y, player) 
					|| isJumpPossible(board, x, y, player));
			opponentHasMoves += (value < 0) && (isSingleStepPossible(board, x, y, !player) 
					|| isJumpPossible(board, x, y, !player));
			score += value;
		}
	}
	if (!hasMoves){
		return -100;
	}	
	if (!opponentHasMoves){
		return 100;
	}	
	return score;
}

static int Board_getColorByTile(char** board, struct Tile* tile){
	int x = tile->x-97;
	int y = tile->y-1 ;
	char piece = Board_getPiece(board, x, y);
	if (piece == Board_BLACK_KING || piece == Board_BLACK_MAN){
		return BLACK;
	}
	if (piece == Board_WHITE_KING || piece == Board_WHITE_MAN){
		return WHITE;
	}
	return -1;
}

static void populateJumpList(struct LinkedList* possibleJumps, struct PossibleMove* possibleMove){
	struct Tile* lastStep = PossibleMove_getLastTile(possibleMove);
	char** board = possibleMove->board;
	int x = (int)(lastStep->x-96);
	int y = lastStep->y;
	
	//checking if another jump is possible after current last jump
	int found = 0;
	for (int i = -1; i <= 1; i += 2){
		for (int j = -1; j <= 1; j += 2){
			if (!isInRange(x+i,y+j) || !isInRange(x+2*i,y+2*j)){
				continue;
			}
			struct Tile* currentLastTile = PossibleMove_getLastTile(possibleMove);
			int player = Board_getColorByTile(possibleMove->board, currentLastTile);
			int enemyNearby = (Board_evalPiece(board, x+i, y+j, player) < 0);
			int enemyIsCapturable = Board_isEmpty(board,x+2*i, y+2*j);		
			if(enemyNearby && enemyIsCapturable){ //found another possible jump after current last step
				struct PossibleMove* currentMoveClone = PossibleMove_clone(possibleMove);
				struct LinkedList* currentSteps = currentMoveClone->steps;
				struct Tile* extraTile = Tile_new(x+2*i, y+2*j);
				LinkedList_add(currentSteps, extraTile);
				
				char oldX = currentLastTile->x;
				int oldY = currentLastTile->y;
				char newX = extraTile->x;
				char newY = extraTile->y;
				Board_move(currentMoveClone->board, oldX, oldY, newX, newY);
				
				populateJumpList(possibleJumps, currentMoveClone);			
				found = 1;
			}
		}
	}
	if (found){
		PossibleMove_free(possibleMove);
	}
	else{
		LinkedList_add(possibleJumps, possibleMove);
	}	
}

/*
 * Gets a list of all jump moves currently possible for a player.
 *
 * @params: (board) - the current state of the playing board
 *			(player) - the player whose moves are to be put in the list
 * @return: a LinkedList struct of jump moves currently possible for the player 
 */
static struct LinkedList* getPossibleJumps (char** board, int player){
	struct LinkedList* jumpMoves = LinkedList_new(&PossibleMove_free);
	
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){			
			if (Board_evalPiece(board, x, y, player) <= 0){
				continue;
			}
			for (int i = -1; i <= 1; i += 2){
				for (int j = -1; j <= 1; j += 2){
					if (!isInRange(x+i,y+j) || !isInRange(x+2*i,y+2*j)){
						continue;
					}
					int enemyNearby = Board_evalPiece(board, x+i+1, y+j+1, player) < 0;
					int enemyIsCapturable = Board_isEmpty(board, x+2*i, y+2*j);
					if(enemyNearby && enemyIsCapturable){
						struct Tile* destTile = Tile_new(x+2*i, y+2*j);
						struct LinkedList* individualJumpMoves = LinkedList_new(&Tile_free);
						LinkedList_add(individualJumpMoves, destTile);
						struct PossibleMove* possibleJumpMove = PossibleMove_new(x, y, individualJumpMoves, board);
						populateJumpList(jumpMoves, possibleJumpMove);
					}
				}
			}
		}	
	}
	return jumpMoves;
}

/*
 * Gets a list of all single step moves currently possible for a player.
 *
 * @params: (board) - the current state of the playing board
 *			(player) - the player whose moves are to be put in the list
 * @return: a LinkedList struct of single step moves currently possible for the player 
 */
static struct LinkedList* getPossibleSingleMoves (char** board, int player){
	struct LinkedList* possibleSingleMoves = LinkedList_new(&PossibleMove_free);
	int forward = (player == BLACK) ? -1 : 1; /* for each player's different direction of "forward" */
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			if (Board_evalPiece(board, x, y, player) <= 0){ //this tile doesn't contain one of this player's pieces
				continue;
			} 
			for (int i = -1; i <= 1; i += 2){
				if (!isInRange(x+i, y+forward)){
					continue;
				}
				if(Board_isEmpty(board, x+i, y+forward)){
					struct Tile* destTile = Tile_new(x+i, y+forward);
					struct LinkedList* singleSteps = LinkedList_new(&Tile_free);
					LinkedList_add(singleSteps, destTile);
					struct PossibleMove* possibleSingleMove = PossibleMove_new(x, y, singleSteps, board);
					LinkedList_add(possibleSingleMoves, possibleSingleMove);
				}
			}
		}
	}
	return possibleSingleMoves;
}

/*
 * Main function for getting all of the moves currently possible for a player. 
 *
 * @params: (board) - the current state of the playing board
 *			(player) - the player whose moves are to be put in the list
 * @return: a LinkedList struct of all moves currently possible for the player 
 */
struct LinkedList* Board_getPossibleMoves(char** board, int player){
	struct LinkedList* possibleJumpMoves = getPossibleJumps(board, player);
	if (LinkedList_length(possibleJumpMoves) != 0){ /* if jumps are possible, they are the only type of move legally possible */
		int maxCaptures = 0;
		
		struct Iterator iterator;
		Iterator_init(&iterator, possibleJumpMoves);
		while(Iterator_hasNext(&iterator)){ //finding the maximum number of possible captures in a single move
			struct PossibleMove* currMove = (struct PossibleMove*)(Iterator_next(&iterator));
			int currNumOfCaptures = PossibleMove_numOfCaptures(currMove);
			if(currNumOfCaptures > maxCaptures){
				maxCaptures = currNumOfCaptures;
			}
		}			
		
		//trimming the possible jump moves list so it contains only jumps that result in the maximum number of captures
		struct LinkedList* trimmedJumpMoves = LinkedList_new(&PossibleMove_free);
		struct Iterator secondIterator;
		Iterator_init(&secondIterator, possibleJumpMoves);
		while(Iterator_hasNext(&secondIterator)){
			struct PossibleMove* currMove = (struct PossibleMove*)(Iterator_next(&secondIterator));
			if(PossibleMove_numOfCaptures(currMove) == maxCaptures){
				LinkedList_add(trimmedJumpMoves, currMove);
			}
		}
		return trimmedJumpMoves;
	}
	LinkedList_free(possibleJumpMoves);
	struct LinkedList* singleMoves = getPossibleSingleMoves(board, player);
	return singleMoves;
}	

/*
 * Auxiliary function for printing the lines as part of printing the playing board.
 */
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

/*
 * Frees the structure.
 */
void Board_free(char** board){
	for(int i = 0; i < Board_SIZE; i++){
		free(board[i]);
	}
	free(board);
}
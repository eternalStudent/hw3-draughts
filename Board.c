#include <stdio.h>
#include "Board.h"
#define BLACK 0
#define WHITE 1

/*
 * Creates a new board structure.
 *
 * @return:  NULL if an allocation error occurred, the new board structured as a two-dimensional char array otherwise
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
 * @params: (dest) - a pointer to the board to be populated, 
 *          (src)  - a pointer to the board according to whom (dest) will be populated
 */
void Board_copy(char** dest, char** src){
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			dest[x][y] = src[x][y];
		}
	}
}

/*
 * Places a piece in a specified position on the board.
 *
 * @params: (x, y)  - the coordinates of the position to be populated
 *          (piece) - the piece to be placed on the board.
 */
void Board_setPiece(char** board, int x, int y, char piece){
	board[x-1][y-1] = piece;
}

/*
 * Retrieves a piece from a specified position on the board.
 *
 * @params: (x, y)  - the coordinates of the position from which to retrieve the piece
 * @return: the piece in the specified position
 */
char Board_getPiece(char** board, int x, int y){
	return board[x-1][y-1];
}

/*
 * Removes a piece from a specified position on the board.
 *
 * @params: (x, y) - the coordinates of the position from which the piece will be removed
 * @return: the removed piece
 */
char Board_removePiece(char** board, int x, int y){
	char piece = Board_getPiece(board, x, y);
	board[x-1][y-1] = Board_EMPTY;
	return piece;
}

/*
 * Checks whether the input coordinates are within the range of the board's length and width.
 *
 * @params: (x, y) - the coordinates to be checked
 * @return: 1 (ture) if the coordinates correspond to a valid position on the board, 0 (false) otherwise
 */
static int isInRange(int x, int y){
	if (x >= 1 && x <= Board_SIZE && y >= 1 && y <= Board_SIZE){
		return 1;
	}
	return 0;
}

/*
 * Checks whether the input coordinates correspond to a black tile on the board.
 *
 * @params: (x, y) - the coordinates to be checked
 * @return: 1 (true) if the coordinates correspond to a black tile on the board, 0 (false) otherwise
 */
static int isOnBlack(int x, int y){
	return !((x+y)%2);
}

/*
 * Checks whether the input coordinates correspond to an empty tile on the board.
 *
 * @params: (x, y) - the coordinates to be checked
 * @return: 1 (true) if the coordinates correspond to an empty tile on the board, 0 (false) otherwise
 */
int Board_isEmpty(char** board, int x, int y){
	return board[x-1][y-1] == Board_EMPTY;
}

/*
 * Checks whether the input coordinates correspond to a valid tile position on the board.
 * That is, a position that is both within range and black.
 *
 * @params: (x, y) - the coordinates to be checked
 * @return: 1 (true) if the coordinates correspond to a valid position on the board, 0 (false) otherwise
 */
int Board_isValidPosition(char** board, int x, int y){
	return (isInRange(x,y) && isOnBlack(x,y));
}

/*
 * Checks whether the input board is playable. Specifically, checks that the board is not empty,
 * has pieces of both colors, and that no color has over 20 pieces.  
 *
 * @params: (board) - the board to be checked		
 * @return: 1 (true) if the board is playable, 0 (false) otherwise
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
 * @params: (oldX, oldY) - the coordinates of the piece to be moved
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
static int Board_move(char** board, int oldX, int oldY, int newX, int newY){
	char piece = Board_getPiece(board, oldX, oldY);
	Board_removePiece(board, oldX, oldY);
	Board_setPiece(board, newX, newY, piece);
	
	Board_crownPieces(board);
	Board_removeCaptured(board, oldX, oldY, newX, newY);
	if (piece != Board_getPiece(board, oldX, oldY)){ // piece became king after this move
		return 1;
	}
	return 0;
}

/*
 * Updates a board according to a possible move.
 *
 * @params: (move) - the move to be carried out on the board 
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
 * @params: (possibleMove) - a pointer to the move to be carried out.
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
 * @params: (x,y) - the coordinates of the piece to be evaluated
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
 * Determines the color of a piece in a given position.
 *
 * @params: (x, y) the coordinates of the given position
 * @return: -1 if the position is empty, the color of the piece otherwise
 */
static int Board_getColor(char** board, int x, int y){
	char piece = Board_getPiece(board, x, y);
	if (piece == Board_BLACK_KING || piece == Board_BLACK_MAN){
		return BLACK;
	}
	if (piece == Board_WHITE_KING || piece == Board_WHITE_MAN){
		return WHITE;
	}
	return -1;
}

/*
 * Determines the color of a piece in a given tile.
 *
 * @params: (tile) - a pointer to the tile on which the piece is
 * @return: -1 if the position is empty, the color of the piece otherwise
 */
static int Board_getColorByTile(char** board, struct Tile* tile){
	return Board_getColor(board, tile->x, tile->y);
}

/*
 * Checks whether a king in a given position can capture an enemy in a given direction
 *
 * @params: (x, y) - the coordinates of the given position
 *          (dirX, dirY) - the coordinates if the direction
 * @return: NULL if no such enemy exists, the tile of the enemy otherwise
 */
static struct Tile* canKingCaptureInDirection(char** board, int x, int y, int dirX, int dirY){
	int player = Board_getColor(board, x, y);
	int i = 1;
	int foundFirstEnemyInThisDirection = 0;
	while(isInRange(x+(i+1)*dirX, y+(i+1)*dirY)){
		int enemyNearby = Board_evalPiece(board, x+i*dirX, y+i*dirY, player)<0;
		int enemyIsCapturable = Board_isEmpty(board, x+(i+1)*dirX, y+(i+1)*dirY);
		if (enemyNearby && enemyIsCapturable && !foundFirstEnemyInThisDirection){
			return Tile_new(x+(i+1)*dirX, y+(i+1)*dirY);
		}
		if (enemyNearby){
			foundFirstEnemyInThisDirection = 1; // to eliminate the possibility of capturing several pieces in one jump
		}
		i++;
	}
	return NULL;
}

/*
 * Checks whether a piece currently has any possible single step moves.
 *
 * @params: (x,y) - the coordinates of the piece to be checked
 *			(player) - the player the check is done for
 * @return: 1 (true) if the piece has a possible single step move, 0 (false) otherwise
 */
static int isSingleStepPossible(char** board, int x, int y, int player){
	int forward = (player == BLACK)? -1 : 1;
	if (Board_evalPiece(board, x, y, player) <= 0){
		return 0;
	} 
	for (int sideward = -1; sideward <= 1; sideward += 2){
		for (int k = -Board_SIZE; k <= Board_SIZE; k++){
			if (!isInRange(x+k*sideward, y+k)){
				continue;
			}
			int pieceIsKing = Board_evalPiece(board, x, y, player) == 3;
			if (!pieceIsKing && k != forward){
				continue;
			}
			if(Board_isEmpty(board, x+k*sideward, y+k)){
				return 1;
			}
			if (!pieceIsKing && k == forward){
				break;
			}
		}
	}
	return 0;
}

/*
 * Checks whether a piece currently has any possible jump moves.
 *
 * @params: (x,y) - the coordinates of the piece to be checked
 *			(player) - the player the check is done for
 * @return: 1 (true) if the piece has a possible jump move, 0 (false) otherwise
 */
static int isJumpPossible(char** board, int x, int y, int player){
	if (Board_evalPiece(board, x, y, player) <= 0){
		return 0;
	}
	for (int i = -1; i <= 1; i += 2){
		for (int j = -1; j <= 1; j += 2){
			if (!isInRange(x+2*i,y+2*j)){
				continue;
			}
			int pieceIsKing = Board_evalPiece(board, x, y, player) == 3;
			if (pieceIsKing){
				struct Tile* dest = canKingCaptureInDirection(board, x, y, i, j);
				if (dest){
					Tile_free(dest);
					return 1;
				}
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
			if (value == 0){
				continue;
			}
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

/*
 * Populates the list of possible jumps, recursively.
 *
 * @params: (possibleJumps) - the list to be populated
 *          (possibleMove) - the move that parts of it will be populated in the list.
 */
static void populateJumpList(struct LinkedList* possibleJumps, struct PossibleMove* possibleMove){
	struct Tile* lastStep = PossibleMove_getLastStep(possibleMove);
	char** board = possibleMove->board;
	int x = lastStep->x;
	int y = lastStep->y;
	
	//checking if another jump is possible after current last jump
	int found = 0;
	for (int i = -1; i <= 1; i += 2){
		for (int j = -1; j <= 1; j += 2){
			if (!isInRange(x+i,y+j) || !isInRange(x+2*i,y+2*j)){
				continue;
			}
			struct Tile* currentLastStep = PossibleMove_getLastStep(possibleMove);
			int player = Board_getColorByTile(possibleMove->board, currentLastStep);
			int enemyNearby = (Board_evalPiece(board, x+i, y+j, player) < 0);
			int enemyIsCapturable = Board_isEmpty(board,x+2*i, y+2*j);
			int justCrowned = (player == WHITE && y == Board_SIZE) || (player == BLACK && y == 1);
			if(enemyNearby && enemyIsCapturable && !justCrowned){ //found another possible jump after current last step
				struct PossibleMove* currentMoveClone = PossibleMove_clone(possibleMove);
				struct LinkedList* currentSteps = currentMoveClone->steps;
				struct Tile* extraTile = Tile_new(x+2*i, y+2*j);
				LinkedList_add(currentSteps, extraTile);
				
				char oldX = currentLastStep->x;
				int oldY = currentLastStep->y;
				int newX = extraTile->x;
				int newY = extraTile->y;
				Board_move(currentMoveClone->board, oldX, oldY, newX, newY);
				
				populateJumpList(possibleJumps, currentMoveClone);			
				found = 1;
			}
		}
	}
	if (found){
		PossibleMove_free(possibleMove);
		return;
	}
	LinkedList_add(possibleJumps, possibleMove);	
}

/*
 * Retrieves a list of all jump moves currently possible for a player.
 *
 * @params: (player) - the player whose moves are to be put in the list
 * @return: a LinkedList struct of jump moves currently possible for the player, or NULL if any allocation errors occurred
 */
static struct LinkedList* getPossibleJumps (char** board, int player){
	struct LinkedList* jumpMoves = LinkedList_new(&PossibleMove_free);
	if(jumpMoves == NULL){ //allocation failed
		return NULL;
	}
	
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){			
			if (Board_evalPiece(board, x, y, player) <= 0){
				continue;
			}
			for (int i = -1; i <= 1; i += 2){
				for (int j = -1; j <= 1; j += 2){
					struct Tile* destTile = NULL;
					if (!isInRange(x+i,y+j) || !isInRange(x+2*i,y+2*j)){
						continue;
					}
					int pieceIsKing = Board_evalPiece(board, x, y, player) == 3;
					if (pieceIsKing){
						destTile = canKingCaptureInDirection(board, x, y, i, j);
					}
					else{
						int enemyNearby = Board_evalPiece(board, x+i, y+j, player) < 0;
						int enemyIsCapturable = Board_isEmpty(board, x+2*i, y+2*j);
						if(enemyNearby && enemyIsCapturable){
							destTile = Tile_new(x+2*i, y+2*j);
							if (destTile == NULL){ // allocation failed
								return NULL;
							}
						}
					}
					if (!destTile){
						continue;
					}
					struct LinkedList* jumpSteps = LinkedList_new(&Tile_free);
					if (jumpSteps == NULL){ // allocation failed
						Tile_free(destTile);
						return NULL;
					}
					LinkedList_add(jumpSteps, destTile);
					struct PossibleMove* possibleJumpMove = PossibleMove_new(x, y, jumpSteps, board);
					if (possibleJumpMove == NULL){ // allocation failed
						Tile_free(destTile);
						LinkedList_free(jumpSteps);
						return NULL;
					}
					populateJumpList(jumpMoves, possibleJumpMove);
				}
			}
		}	
	}
	return jumpMoves;
}

/*
 * Gets a list of all single step moves currently possible for a player.
 *
 * @params: (player) - the player whose moves are to be put in the list
 * @return: a list of single step moves currently possible for the player, or NULL if any allocation errors occurred 
 */
static struct LinkedList* getPossibleSingleMoves (char** board, int player){
	struct LinkedList* possibleSingleMoves = LinkedList_new(&PossibleMove_free);
	if (possibleSingleMoves == NULL){
		return NULL;
	}
	int forward = (player == BLACK) ? -1 : 1; /* for each player's different direction of "forward" */
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			if (Board_evalPiece(board, x, y, player) <= 0){ //this tile doesn't contain one of this player's pieces
				continue;
			} 
			for (int sideward = -1; sideward <= 1; sideward += 2){
				for (int k = -Board_SIZE; k <= Board_SIZE; k++){
					if (!isInRange(x+k*sideward, y+k)){
						continue;
					}	
					int pieceIsKing = Board_evalPiece(board, x, y, player) == 3;
					if (!pieceIsKing && k != forward){
						continue;
					}	
					if(Board_isEmpty(board, x+k*sideward, y+k)){
						struct Tile* destTile = Tile_new(x+k*sideward, y+k);
						if (destTile == NULL){ // allocation failed
							return NULL;
						}
						struct LinkedList* singleSteps = LinkedList_new(&Tile_free);
						if (singleSteps == NULL){ // allocation failed
							Tile_free(destTile);
							return NULL;
						}
						LinkedList_add(singleSteps, destTile);
						struct PossibleMove* possibleSingleMove = PossibleMove_new(x, y, singleSteps, board);
						if (possibleSingleMove == NULL){ // allocation failed
							Tile_free(destTile);
							LinkedList_free(singleSteps);
							return NULL;
						}
						LinkedList_add(possibleSingleMoves, possibleSingleMove);
					}
					if (!pieceIsKing && k == forward){
						break;
					}
				}
			}
		}
	}
	return possibleSingleMoves;
}

/*
 * Trims the list of possible jump moves, so it only contains moves that result in the maximum amount of captures
 *
 * @params: (jumpMovesList) - the list of all possible jump moves
 * @return: a trimmed list, or NULL if any allocation errors occurred 
 */
static struct LinkedList* trimJumpMovesList (struct LinkedList* jumpMovesList){
	int maxCaptures = 0;
	
	//finding the maximum number of captures in a currently possible jump move
	struct Iterator iterator;
	Iterator_init(&iterator, jumpMovesList);
	while(Iterator_hasNext(&iterator)){ 
		struct PossibleMove* currMove = (struct PossibleMove*)(Iterator_next(&iterator));
		int currNumOfCaptures = PossibleMove_numOfCaptures(currMove);
		if(currNumOfCaptures > maxCaptures){
			maxCaptures = currNumOfCaptures;
		}
	}			
	
	//creating a new list and filling it only with the appropriate moves
	struct LinkedList* trimmedJumpMoves = LinkedList_new(&PossibleMove_free);
	if(trimmedJumpMoves == NULL){ // allocation failed
		return NULL;
	}
	struct Iterator secondIterator;
	Iterator_init(&secondIterator, jumpMovesList);
	while(Iterator_hasNext(&secondIterator)){
		struct PossibleMove* currMove = (struct PossibleMove*)(Iterator_next(&secondIterator));
		if(PossibleMove_numOfCaptures(currMove) == maxCaptures){
			struct PossibleMove* clonedCurrMove = PossibleMove_clone(currMove);
			if (clonedCurrMove == NULL){ // allocation failed
				LinkedList_free(trimmedJumpMoves);
				return NULL;
			}
			LinkedList_add(trimmedJumpMoves, clonedCurrMove);
		}
	}
	LinkedList_free(jumpMovesList);
	return trimmedJumpMoves;		
}

/*
 * Main function for getting all of the moves currently possible for a player. 
 *
 * @params: (player) - the player whose moves are to be put in the list
 * @return: a list of all moves currently possible for the player, or NULL if any allocation errors occurred 
 */
struct LinkedList* Board_getPossibleMoves(char** board, int player){
	struct LinkedList* possibleJumpMoves = getPossibleJumps(board, player);
	if (possibleJumpMoves == NULL){ // allocation failed
		return NULL;
	}
	if (LinkedList_length(possibleJumpMoves) != 0){ /* if jumps are possible, they are the only type of move legally possible */
		struct LinkedList* trimmedJumpMoves = trimJumpMovesList(possibleJumpMoves);
		if(trimmedJumpMoves == NULL){ // allocation failed
			LinkedList_free(possibleJumpMoves);
			return NULL;
		}
		return trimmedJumpMoves;
	}
	LinkedList_free(possibleJumpMoves);
	struct LinkedList* singleMoves = getPossibleSingleMoves(board, player);
	if (singleMoves == NULL){ // allocation failed
		return NULL;
	}
	
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
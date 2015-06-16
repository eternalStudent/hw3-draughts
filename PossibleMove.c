#include "Board.c"

/* 
 * Creates a new PossibleMove structure, consisting of the starting tile,
 * a list of tiles that are part of the move itself, and the state of the
 * board after this particular move has been carried out. 
 *
 * @params: start - a pointer to the starting tile, 
            moves - a pointer to the list of individual tile moves, 
 *          board - the board before the move
 * @return: NULL if any allocation errors occurred, the structure otherwise
 */
struct PossibleMove* PossibleMove_new(int x, int y, struct LinkedList* moveList, char** board){
	struct PossibleMove* move;
	move = (struct PossibleMove*)calloc(1, sizeof(struct PossibleMove));
	if (!move){
		return NULL;
	}	
	move->start = Tile_new(x, y);
	if (!(move->start)){
		free(move);
		return NULL;
	}
	move->moves = moveList;
	move->board = Board_getPossibleBoard(board, move);
	return move;
}

int PossibleMove_equals(struct PossibleMove* this, struct PossibleMove* other){
	int tile_equals = Tile_equals(this->start, other->start);
	if (!tile_equals || LinkedList_length(this->moves) != LinkedList_length(other->moves)){
		return 0;
	}
	struct Iterator it1;
	Iterator_init(&it1, this->moves);
	struct Iterator it2;
	Iterator_init(&it2, other->moves);
	while (Iterator_hasNext(&it1)){
		struct Tile* curr1 = (struct Tile*)Iterator_next(&it1);
		struct Tile* curr2 = (struct Tile*)Iterator_next(&it2);
		if (!Tile_equals(curr1, curr2)){
			return 0;
		}	
	}
	return 1;
}

/* 
 * Concatenates two consecutive, contiguous possible moves.
 * Example: (a->b) + (b->c) = a->b->c
 */ 
struct PossibleMove* PossibleMove_concatenate(struct PossibleMove* initialMove, struct PossibleMove* nextMove, char** currentBoard){
	struct Tile* initialTile = initialMove->start;
	struct LinkedList* newMoveList = LinkedList_new(&Tile_free);
	struct Tile* middleTile = nextMove->start;
	LinkedList_add(newMoveList,middleTile);	
	
	struct LinkedList* nextMoveList = nextMove->moves;
	struct Iterator iterator;
	Iterator_init(&iterator, nextMoveList);
	while(Iterator_hasNext(&iterator)){
		struct Tile* extraTile = (struct Tile*)(Iterator_next(&iterator));
		LinkedList_add(newMoveList, extraTile);
	}
	struct PossibleMove* resMove = PossibleMove_new((initialTile->x)-96,initialTile->y, newMoveList, currentBoard);
	
	return resMove;
}

/* 
 * Concatenates a single Possible Move with a list of possible moves that can be performed after it 
 * Example: (a->b) + [(b->c), (b->d), (b->e)] = [(a->b->c), (a->b->d), (a->b->e)]
 */ 
struct LinkedList* PossibleMove_concatenateWithList(struct PossibleMove* initialMove, struct LinkedList* followingMoves, char** currentBoard){
	struct LinkedList* resMoveList = LinkedList_new(&PossibleMove_free);
	struct Iterator iterator;
	Iterator_init(&iterator, followingMoves);
	while(Iterator_hasNext(&iterator)){
		struct PossibleMove* currMove = (struct PossibleMove*)(Iterator_next(&iterator));
		struct PossibleMove* concatenatedMove = PossibleMove_concatenate(initialMove,currMove,currentBoard);
		LinkedList_add(resMoveList, concatenatedMove);
	}
	return resMoveList;
}

/* 
 * Prints the structure in the format: "move <x,y> to <i,j>[<k,l>...]".
 */
void PossibleMove_print(struct PossibleMove* move){
	printf("move ");
	Tile_print(move->start);
	printf(" to ");
	struct Iterator iterator;
	Iterator_init(&iterator, move->moves);
	while (Iterator_hasNext(&iterator)){
		struct Tile* tile = (struct Tile*)Iterator_next(&iterator);
		Tile_print(tile);
	}
}

struct Tile* PossibleMove_getLastTile(struct PossibleMove* move){
	struct LinkedList* moveList = move->moves;
	struct Tile* lastTile = (struct Tile*)(moveList->last->data);
	return lastTile;
}

struct PossibleMove* PossibleMove_clone (struct PossibleMove* move){
	struct PossibleMove* clonedMove;
	clonedMove = (struct PossibleMove*)calloc(1, sizeof(struct PossibleMove));
	if (!clonedMove){
		return NULL;
	}	
	
	struct Tile* originalStart = move->start;
	struct Tile* clonedStart = Tile_clone(originalStart);
	if (!(clonedStart)){
		free(clonedMove);
		return NULL;
	}
	clonedMove->start = clonedStart;
	
	struct LinkedList* originalMoveList = move->moves;
	struct LinkedList* clonedMoveList = LinkedList_new(&Tile_free);
	
	struct Iterator iterator;
	Iterator_init(&iterator,originalMoveList);
	while(Iterator_hasNext(&iterator)){
		struct Tile* currOriginalTile = (struct Tile*)(Iterator_next(&iterator));
		struct Tile* clonedTile = Tile_clone(currOriginalTile);
		if (!clonedTile){
			free(clonedMove);
			Tile_free(clonedStart);
			LinkedList_free(clonedMoveList);
			return NULL;
		}
		LinkedList_add(clonedMoveList, clonedTile);
	}
	clonedMove->moves = clonedMoveList;
	
	char** clonedBoard = Board_new();
	if (!clonedBoard){
		free(clonedMove);
		Tile_free(clonedStart);
		LinkedList_free(clonedMoveList);
		return NULL;
	}
	Board_copy(clonedBoard,move->board);
	clonedMove->board = clonedBoard;
	return clonedMove;
}

int PossibleMove_numOfCaptures(struct PossibleMove* move){
	return LinkedList_length(move->moves);
}


/* 
 * Frees the structure.
 */
void PossibleMove_free(void* data){
	struct PossibleMove* move = (struct PossibleMove*) data;
	Tile_free(move->start);
	LinkedList_free(move->moves);
	Board_free(move->board);
	free(move);
}
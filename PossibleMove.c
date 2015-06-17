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
struct PossibleMove* PossibleMove_new(int x, int y, struct LinkedList* steps, char** board){
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
	move->steps = steps;
	move->board = Board_getPossibleBoard(board, move);
	return move;
}

/*
 * Checks whether a PossibleMove struct represents the same move.
 *
 * @params: (other) - a pointer to the struct to be checked
 * @return: 1 (true) if both of the tiles represent the same move, 0 (false) otherwise
 */
int PossibleMove_equals(struct PossibleMove* this, struct PossibleMove* other){
	int tile_equals = Tile_equals(this->start, other->start);
	if (!tile_equals || LinkedList_length(this->steps) != LinkedList_length(other->steps)){
		return 0;
	}
	struct Iterator it1;
	Iterator_init(&it1, this->steps);
	struct Iterator it2;
	Iterator_init(&it2, other->steps);
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
 * Prints the structure in the format: "move <x,y> to <i,j>[<k,l>...]".
 */
void PossibleMove_print(struct PossibleMove* move){
	printf("move ");
	Tile_print(move->start);
	printf(" to ");
	struct Iterator iterator;
	Iterator_init(&iterator, move->steps);
	while (Iterator_hasNext(&iterator)){
		struct Tile* tile = (struct Tile*)Iterator_next(&iterator);
		Tile_print(tile);
	}
}

/*
 * @return: the last step of the move
 */
struct Tile* PossibleMove_getLastStep(struct PossibleMove* move){
	struct LinkedList* moveList = move->steps;
	struct Tile* lastStep = (struct Tile*)(moveList->last->data);
	return lastStep;
}

/*
 * Deep-clones the move.
 *
 * @return: NULL if any allocation errors occurred, the cloned tile otherwise
 */
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
	
	struct LinkedList* originalMoveList = move->steps;
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
	clonedMove->steps = clonedMoveList;
	
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
	return LinkedList_length(move->steps);
}

/* 
 * Frees the structure.
 */
void PossibleMove_free(void* data){
	struct PossibleMove* move = (struct PossibleMove*) data;
	Tile_free(move->start);
	LinkedList_free(move->steps);
	Board_free(move->board);
	free(move);
}
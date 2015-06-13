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
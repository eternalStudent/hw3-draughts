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
struct PossibleMove* PossibleMove_new(struct Tile* start, struct LinkedList* list, char** board){
	struct PossibleMove* move;
	move = (struct PossibleMove*)calloc(1, sizeof(struct PossibleMove));
	if (!move){
		return NULL;
	}
	move->start = start;
	move->moves = list;
	move->board = Board_getPossibleBoard(board, move);
	return move;
}

/* 
 * Prints the structure in the format: "move <x,y> to <i,j>[<k,l>...]".
 */
void PossibleMove_print(struct PossibleMove* move){
	printf("move ");
	Tile_print(move->start);
	printf(" to ");
	struct Iterator* iterator = Iterator_new(move->moves);
	while (Iterator_hasNext(iterator)){
		struct Tile* tile = (struct Tile*)Iterator_next(iterator);
		Tile_print(tile);
	}
	printf("\n");
	Iterator_free(iterator);
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
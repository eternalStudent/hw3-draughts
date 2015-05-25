#include "PossibleMove.h"

/* 
 * creates a new PossibleMmove structure, consisting of the starting tile,
 * a list of tiles that are part of the move itself, and the state of the
 * board after this particular move has been carried out. 
 */

struct PossibleMove* PossibleMove_new(struct Tile* start, struct LinkedList* list, char board[Board_SIZE][Board_SIZE]){
	struct PossibleMove* move;
	move = (struct PossibleMove*)calloc(1, sizeof(struct PossibleMove));
	if (!move){
		return NULL;
	}
	move->start = start;
	move->moves = list;
	move->board = board;
	return move;
}

/* 
 *prints an existing PossibleMmove structure in the correct format: move <x,y> to <i,j>[<k,l>...]
 */


void PossibleMove_print(struct PossibleMove* move){
	printf("move ");
	Tile_print(move->start);
	printf(" to ");
	struct Iterator* iterator = Iterator_new(list);
	while (Iterator_hasNext(iterator)){
		struct Tile* tile = (struct Tile*)Iterator_next(iterator);
		Tile_print(tile);
	}
	printf("\n");
	Iterator_free(iterator);
}

/* 
 *frees all memory allocations of a given PossibleMove structure
 */

void PossibleMove_free(void* data){
	struct PossibleMove* move = (struct PossibleMove*) data;
	Tile_free(move->start);
	LinkedList_free(move->moves);
	free(move);
}
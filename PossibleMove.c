#include "PossibleMove.h"


struct PossibleMove* PossibleMove_new(struct Tile* start, struct LinkedList* list, char board[Board_SIZE][Board_SIZE]){
	/* generate all possible moves, depending on start and board, and store them in list */
}

void PossibleMove_print(struct PossibleMove* move){
	printf("move ");
	Tile_print(move->start);
	printf(" to ");
	LinkedList_print(move->list);
	printf("\n");
}

void PossibleMove_free(void* data){
	struct PossibleMove* move = (struct PossibleMove*) data;
	Tile_free(move->start);
	LinkedList_free(move->moves);
	free(move->board);
	free(move);
}
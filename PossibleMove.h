#include "Tile.c"


struct PossibleMove{
		struct Tile* start;
		struct LinkedList* moves;
		char board[Board_SIZE][Board_SIZE] ;
};

struct PossibleMove* PossibleMove_new(struct Tile*, struct LinkedList*, char[Board_SIZE][Board_SIZE]);
void PossibleMove_print(struct PossibleMove*);
void PossibleMove_free(void*);
void PossibleMoveList_print(struct LinkedList*);
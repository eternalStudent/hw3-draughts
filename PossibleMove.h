#include "Tile.c"


struct PossibleMove{
	struct Tile* start;
	struct LinkedList* moves;
	char** board;
};

struct PossibleMove* PossibleMove_new(struct Tile*, struct LinkedList*, char**);

void PossibleMove_print(struct PossibleMove*);

void PossibleMove_free(void*);

void PossibleMoveList_print(struct LinkedList*);
#include "Tile.c"


struct PossibleMove{
	struct Tile* start;
	struct LinkedList* moves;
	char** board;
};

struct PossibleMove* PossibleMove_new(int, int, struct LinkedList*, char**);

int PossibleMove_equals(struct PossibleMove* this, struct PossibleMove* other);

void PossibleMove_print(struct PossibleMove*);

void PossibleMove_free(void*);

void PossibleMoveList_print(struct LinkedList*);
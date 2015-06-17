#include "Tile.c"


struct PossibleMove{
	struct Tile* start;
	struct LinkedList* steps;
	char** board;
};

struct PossibleMove* PossibleMove_new(int, int, struct LinkedList*, char**);

int PossibleMove_equals(struct PossibleMove* this, struct PossibleMove* other);

void PossibleMove_print(struct PossibleMove*);

struct Tile* PossibleMove_getLastStep(struct PossibleMove* move);

struct PossibleMove* PossibleMove_clone (struct PossibleMove* move);

void PossibleMove_free(void*);

void PossibleMoveList_print(struct LinkedList*);
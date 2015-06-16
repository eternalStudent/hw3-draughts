#include "Tile.c"


struct PossibleMove{
	struct Tile* start;
	struct LinkedList* steps;
	char** board;
};

struct PossibleMove* PossibleMove_new(int, int, struct LinkedList*, char**);

int PossibleMove_equals(struct PossibleMove* this, struct PossibleMove* other);

struct PossibleMove* PossibleMove_concatenate(struct PossibleMove* initialMove, struct PossibleMove* nextMove, char** currentBoard);

struct LinkedList* PossibleMove_concatenateWithList(struct PossibleMove* initialMove, struct LinkedList* followingMoves, char** currentBoard);

void PossibleMove_print(struct PossibleMove*);

struct Tile* PossibleMove_getLastTile(struct PossibleMove* move);

struct PossibleMove* PossibleMove_clone (struct PossibleMove* move);

int PossibleMove_numOfCaptures(struct PossibleMove* move);

void PossibleMove_free(void*);

void PossibleMoveList_print(struct LinkedList*);
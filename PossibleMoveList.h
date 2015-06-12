#include "PossibleMove.c"

struct LinkedList* PossibleMoveList_new();

void PossibleMoveList_print(struct LinkedList* list);

int PossibleMoveList_contains(struct LinkedList* list, struct PossibleMove* move);

void PossibleMoveList_free(struct LinkedList* list);
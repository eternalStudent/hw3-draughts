#include "PossibleMoveList.h"

struct LinkedList* PossibleMoveList_new(){
	return LinkedList_new(&PossibleMove_free);
}

struct PossibleMove* PossibleMoveList_first(struct LinkedList* list){
	return (struct PossibleMove*)LinkedList_first(list);
}

void PossibleMoveList_print(struct LinkedList* list){
	struct Iterator iterator;
	Iterator_init(&iterator, list);
	while(Iterator_hasNext(&iterator)){
		struct PossibleMove* move = (struct PossibleMove*)Iterator_next(&iterator);
		PossibleMove_print(move);
		printf("\n");
	}
}

int PossibleMoveList_contains(struct LinkedList* list, struct PossibleMove* move){
	struct Iterator iterator;
	Iterator_init(&iterator, list);
	while(Iterator_hasNext(&iterator)){
		struct PossibleMove* current = Iterator_next(&iterator);
		if (PossibleMove_equals(current, move)){
			return 1;
		}
	}
	return 0;
}

void PossibleMoveList_free(struct LinkedList* list){
	LinkedList_free(list);
}
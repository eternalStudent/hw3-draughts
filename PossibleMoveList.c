#include "PossibleMoveList.h"

struct LinkedList* PossibleMoveList_new(){
	return LinkedList_new(&PossibleMove_free);
}

void PossibleMoveList_print(struct LinkedList* list){
	struct Iterator* iterator = Iterator_new(list);
	while(Iterator_hasNext(iterator)){
		struct PossibleMove* move = (struct PossibleMove*)Iterator_next(iterator);
		PossibleMove_print(move);
		printf("\n");
	}
	Iterator_free(iterator);
}

int PossibleMoveList_contains(struct LinkedList* list, struct PossibleMove* move){
	struct Iterator* iterator = Iterator_new(list);
	while(Iterator_hasNext(iterator)){
		struct PossibleMove* current = Iterator_next(iterator);
		if (PossibleMove_equals(current, move)){
			return 1;
		}
	}
	Iterator_free(iterator);
	return 0;
}

void PossibleMoveList_free(struct LinkedList* list){
	LinkedList_free(list);
}
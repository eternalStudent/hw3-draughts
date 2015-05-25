#include "Iterator.h"
#include <stdio.h>

struct Iterator* Iterator_new(struct LinkedList* list){
	struct Iterator* iterator;
	iterator = (struct Iterator*)calloc(1, sizeof(struct Iterator));
	if (!iterator){
		return NULL;
	}
	iterator->first = list->first;
	iterator->current = NULL;
	return iterator;
}
	
void* Iterator_next(struct Iterator* iterator){
	if (iterator->current == NULL){
		iterator->current = iterator->first;
	}
	else{
		iterator->current = iterator->current->next;
	}
	return iterator->current->data;
}

int Iterator_hasNext(struct Iterator* iterator){
	if (iterator->current == NULL){
		if (iterator->first == NULL){
			return 0;
		}
		return 1;
	}
	return (iterator->current->next != NULL);
}

void Iterator_free(struct Iterator* iterator){
	free(iterator);
}
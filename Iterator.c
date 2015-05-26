#include "Iterator.h"
#include <stdio.h>

/* 
 * Creates a new Iterator structure, enabling iteration over a LinkedList structure.
 *
 * @params: list - the list to be iterated over
 * @return: NULL if any allocation errors occurred, the iterator otherwise
 */
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

/* 
 * @return: the next element in the structure over which the iterator is currently working
 */
void* Iterator_next(struct Iterator* iterator){
	if (iterator->current == NULL){
		iterator->current = iterator->first;
	}
	else{
		iterator->current = iterator->current->next;
	}
	return iterator->current->data;
}

/* 
 * @return: 0 (false) if the given iterator has reached the end of the list, 1 (true) otherwise
 */
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
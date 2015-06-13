#include "Iterator.h"
#include <stdio.h>


void Iterator_init(struct Iterator* iterator, struct LinkedList* list){
	iterator->first = list->first;
	iterator->current = NULL;
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
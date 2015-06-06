#include "LinkedList.h"
#include <stdlib.h>
#include <string.h>

/* 
 * Creates a new LinkedList structure.
 *
 * @params: freeFunc - the function that will free the data
 * @return: NULL if any allocation errors occurred, the list otherwise
 */
struct LinkedList* LinkedList_new(void(*freeFunc)(void*)){
	struct LinkedList* list;
	list = (struct LinkedList*)calloc(1, sizeof(struct LinkedList));
	if (!list){
		return NULL;
	}
	list->length = 0;
	list->first = NULL;
	list->last = NULL;
	list->freeFunc = freeFunc;
	return list;
}

/* 
 * Creates a new ListNode structure, containing a pointer to a new element in a list.
 *
 * @params: data - a pointer to the element
 * @return: NULL if any allocation errors occurred, the node otherwise
 */
static struct ListNode* ListNode_new(void* data){
	struct ListNode* node;
	node = (struct ListNode*)calloc(1, sizeof(struct ListNode));
	if (!node){
		return NULL;
	}
	node->data = data;
	return node;
}

/* 
 * Appends a new element to the end of the list.
 * 
 * @params: data - a pointer to the element
 * @return: -1 if any allocation errors occurred, 0 otherwise
 */
int LinkedList_add(struct LinkedList* list, void* data){
	struct ListNode* node = ListNode_new(data);
	if (node == NULL){
		return -1;
	}
	if (list->length == 0){
		list->first = node;
	}
	else{
		list->last->next = node;
	}
	list->length++;
	list->last = node;
	return 0;
}

/* 
 * Concatenates two separate Linked Lists.
 */
int LinkedList_concatenate(struct LinkedList* firstList, struct LinkedList* secondList){
	firstList->last->next = secondList->first;
	firstList->last = secondList->last;
	firstList->length = LinkedList_length(firstList) + LinkedList_length(secondList);
	free(secondList); /* second list structure no longer relevant */
	return 0;
}

/* 
 * @return: the number of elements in the list
 */
int LinkedList_length(struct LinkedList* list){
	return list->length;
}

/* 
 * Frees the list from memory.
 */
void LinkedList_free(struct LinkedList* list){
	struct ListNode* node = list->first;
	while (node != NULL){
		struct ListNode* next = node->next;
		list->freeFunc(node->data);
		free(node);
		node = next;
	}
	free(list);
}
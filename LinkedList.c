#include "LinkedList.h"
#include <stdlib.h>
#include <string.h>

/* 
 * creates a new LinkedList structure
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
 * creates a new ListNode structure, representing a single node of a linked list
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
 * appends to the given list a new node containing the given data
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
 * @return the length of a given list
 */

int LinkedList_length(struct LinkedList* list){
	return list->length;
}

/* 
 * frees up all of the memory allocated to a given list, including all of its nodes
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
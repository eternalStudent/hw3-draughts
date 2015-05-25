#include "LinkedList.h"
#include <stdlib.h>
#include <string.h>

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

static struct ListNode* ListNode_new(void* data){
	struct ListNode* node;
	node = (struct ListNode*)calloc(1, sizeof(struct ListNode));
	if (!node){
		return NULL;
	}
	node->data = data;
	return node;
}

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

int LinkedList_length(struct LinkedList* list){
	return list->length;
}

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

void LinkedList_print(struct LinkedList* list){
	struct Iterator* iterator = Iterator_new(list);
	while (Iterator_hasNext(iterator)){
		struct Tile* tile = (struct Tile*)Iterator_next(iterator);
		Tile_print(tile);
	}
	Iterator_free(iterator);
}
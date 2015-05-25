#include "LinkedList.c"

struct Iterator{
	struct ListNode* first;
	struct ListNode* current;
};


struct Iterator* Iterator_new(struct LinkedList* list);
	
void* Iterator_next(struct Iterator* node);

int Iterator_hasNext(struct Iterator* node);

void Iterator_free(struct Iterator* iterator);
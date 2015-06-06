struct ListNode{
	void* data;
	struct ListNode* next;
};

struct LinkedList{
	struct ListNode* first;
	struct ListNode* last;
	int length;
	void (*freeFunc)(void*);
};

struct LinkedList* LinkedList_new(void(*freeFunc)(void*));

int LinkedList_add(struct LinkedList* list, void* data);

int LinkedList_concatenate(struct LinkedList* firstList, struct LinkedList* secondList);

int LinkedList_length(struct LinkedList* list);

void LinkedList_free(struct LinkedList* list);
#include "Iterator.c"
#include "Tile.c"
#include <stdio.h>

int main(){
	struct LinkedList* list = LinkedList_new(&free);
	struct Tile* a = Tile_new('a', 5);
	LinkedList_add(list, (void*)a);
	struct Tile* b = Tile_new('b', 6);
	LinkedList_add(list, (void*)b);
	LinkedList_print(list);
	LinkedList_free(list);
}

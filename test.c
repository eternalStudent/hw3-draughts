#include "Iterator.c"
#include "Tile.c"
#include <stdio.h>

int main(){
	struct LinkedList* list = LinkedList_new(&free);
	struct Tile* a = Tile_new('a', 5);
	LinkedList_add(list, (void*)a);
	struct Tile* b = Tile_new('b', 6);
	LinkedList_add(list, (void*)b);
	struct Iterator* iterator = Iterator_new(list);
	while (Iterator_hasNext(iterator)){
		struct Tile* tile = (struct Tile*)Iterator_next(iterator);
		Tile_print(tile);
	}
	LinkedList_free(list);
}
#include "Tile.h"
#include <stdio.h>

struct Tile* Tile_new(char x, int y){
	struct Tile* tile;
	tile = (struct Tile*)calloc(1, sizeof(struct Tile));
	if (!tile){
		return NULL;
	}
	tile->x = x;
	tile->y = y;
	return tile;
}

void Tile_print(struct Tile* tile){
	printf("<%c, %d>", tile->x, tile->y);
}

void Tile_free(void* data){
	free(data);
}

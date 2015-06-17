#include "Tile.h"
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

/*
 * Creates a new Tile structure
 *
 * @params: (x, y) - the coordinates of the tile
 * @return: NULL if any allocation errors occurred, the Tile otherwise
 */

struct Tile* Tile_new(int x, int y){
	struct Tile* tile;
	tile = (struct Tile*)calloc(1, sizeof(struct Tile));
	if (!tile){
		return NULL;
	}
	tile->x = x;
	tile->y = y;
	return tile;
}

int Tile_equals(struct Tile* this, struct Tile* other){
	return (this->x == other->x && this->y == other->y);
}

/*
 * Prints the tile in the format: "<x,y>".
 */
void Tile_print(struct Tile* tile){
	printf("<%c,%d>", tile->x+96, tile->y);
}


struct Tile* Tile_clone (struct Tile* tile){
	int x = tile->x;
	int y = tile->y;
	struct Tile* clone = Tile_new(x,y);
	if (!clone){
		return NULL;
	}
	return clone;
}


/*
 * Frees the structure.
 */
void Tile_free(void* data){
	free(data);
}

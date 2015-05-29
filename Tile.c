#include "Tile.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Creates a new Tile structure
 *
 * @params: (x, y) - the coordinates of the tile
 * @return: NULL if any allocation errors occurred, the Tile otherwise
 */
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

/*
 * Prints the tile in the format: "<x,y>".
 */
void Tile_print(struct Tile* tile){
	printf("<%c,%d>", tile->x, tile->y);
}

/*
 * Frees the structure.
 */
void Tile_free(void* data){
	free(data);
}

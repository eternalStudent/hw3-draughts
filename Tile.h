#include <regex.h>

struct Tile{
	char x;
	int y;
};

struct Tile* Tile_new(int x, int y);

int Tile_equals(struct Tile* this, struct Tile* other);

void Tile_print(struct Tile* tile);

void Tile_free(void* data);
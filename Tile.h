struct Tile{
	char x;
	int y;
};

struct Tile* Tile_new(char x, int y);

void Tile_print(struct Tile* tile);

void Tile_free(void* data);
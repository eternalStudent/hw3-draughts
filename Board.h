#include "Iterator.c"
#include "PossibleMove.h"

#define Board_WHITE_MAN  'm'
#define Board_WHITE_KING 'k'
#define Board_BLACK_MAN  'M'
#define Board_BLACK_KING 'K'
#define Board_EMPTY      ' '
#define Board_SIZE       10


char** Board_new();

void Board_init      (char** board);

void Board_clear     (char** board);

void Board_copy      (char** dest, char** src);

int  charToInt       (char x);

int  Board_isEmpty   (char** board, int x, int y);

int  Board_isValidPosition (char** board, int x, int y);

int  Board_isPlayable(char** board);

void Board_update    (char** board, struct PossibleMove* move);

char** Board_getPossibleBoard    (char** board, struct PossibleMove* move);

int Board_isPieceInSpecifiedColor(char** board, int x, int y, int color);

struct LinkedList* Board_getPossibleMoves(char** board, int player);

void Board_print     (char** board);

void Board_free      (char** board);

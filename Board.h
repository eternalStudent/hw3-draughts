#include "Iterator.c"
#include "PossibleMove.h"

#define Board_WHITE_MAN  'm'
#define Board_WHITE_KING 'k'
#define Board_BLACK_MAN  'M'
#define Board_BLACK_KING 'K'
#define Board_EMPTY      ' '
#define Board_SIZE       10


char** Board_new();

void Board_init    (char** board);

void Board_clear   (char** board);

void Board_copy    (char** dest, char** src);

int isInRange      (int x, int y);

int  Board_set     (char** board, char x, int y, char piece);

int  Board_remove  (char** board, char x, int y);

int  Board_isPlayable(char** board);

char** Board_getPossibleBoard(char** board, struct PossibleMove* move);

struct LinkedList* Board_getPossibleMoves(char** board, int player);

int  Board_getScore(char** board);

void Board_print   (char** board);

void Board_free    (char** board);

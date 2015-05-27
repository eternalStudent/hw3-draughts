#define Board_WHITE_MAN  'm'
#define Board_WHITE_KING 'k'
#define Board_BLACK_MAN  'M'
#define Board_BLACK_KING 'K'
#define Board_EMPTY      ' '
#define Board_SIZE       10

typedef char[Board_size][Board_Size] Board;


void Board_init    (char** board);

void Board_clear   (char** board);

void Board_copy    (char** dest, char** src)

int  Board_set     (char** board, char x, int y, char piece);

int  Board_remove  (char** board, char x, int y);

int  Board_move    (char** board, char oldX, int oldY, char newX, int newY);

int  Board_getScore(char** board);

void Board_print   (char** board);

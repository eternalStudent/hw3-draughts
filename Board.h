#define Board_WHITE_MAN  'm'
#define Board_WHITE_KING 'k'
#define Board_BLACK_MAN  'M'
#define Board_BLACK_KING 'K'
#define Board_EMPTY      ' '
#define Board_SIZE       10


int  Board_init    (char[Board_SIZE][Board_SIZE] board);

void Board_clear   (char[Board_SIZE][Board_SIZE] board);

int  Board_set     (char[Board_SIZE][Board_SIZE] board, char x, int y, char piece);

int  Board_remove  (char[Board_SIZE][Board_SIZE] board, char x, int y);

int  Board_move    (char[Board_SIZE][Board_SIZE] board, char oldX, int oldY, char newX, int newY);

int  Board_getScore(char[Board_SIZE][Board_SIZE] board);

void Board_print   (char[Board_SIZE][Board_SIZE] board);

#define Board_WHITE_MAN  'm'
#define Board_WHITE_KING 'k'
#define Board_BLACK_MAN  'M'
#define Board_BLACK_KING 'K'
#define Board_EMPTY      ' '
#define Board_SIZE       10


void Board_init    (char board[Board_SIZE][Board_SIZE]);

void Board_clear   (char board[Board_SIZE][Board_SIZE]);

int  Board_set     (char board[Board_SIZE][Board_SIZE], char x, int y, char piece);

int  Board_remove  (char board[Board_SIZE][Board_SIZE], char x, int y);

int  Board_move    (char board[Board_SIZE][Board_SIZE], char oldX, int oldY, char newX, int newY);

int  Board_getScore(char board[Board_SIZE][Board_SIZE]);

void Board_print   (char board[Board_SIZE][Board_SIZE]);

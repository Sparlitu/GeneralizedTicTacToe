#ifndef GENERALIZEDTICTACTOE_BOARD_H
#define GENERALIZEDTICTACTOE_BOARD_H

typedef struct {
    int size;
    char **cells;
} Board;

Board createBoard(int size);

Board copyBoard(Board *original);

void initializeBoard(Board *board);

void printBoard(Board *board);

void freeBoard(Board *board);

int isBoardFull(Board *board);

#endif //GENERALIZEDTICTACTOE_BOARD_H
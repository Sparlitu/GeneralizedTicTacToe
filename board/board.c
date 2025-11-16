#include "board.h"

#include <stdio.h>
#include <stdlib.h>

// Constructor-like function to create a new board
Board createBoard(int size) {
    Board board;
    board.size = size;
    board.cells = (char **)malloc(board.size * sizeof(char *));
    for (int i = 0; i < board.size; i++) {
        board.cells[i] = (char *)malloc(board.size * sizeof(char));
    }

    return board;
}

Board copyBoard(Board *original) {
    Board newBoard = createBoard(original->size);
    for(int i = 0; i < original->size; i++) {
        for(int j = 0; j < original->size; j++) {
            newBoard.cells[i][j] = original->cells[i][j];
        }
    }
    return newBoard;
}

// Function to initialize the board with empty spaces
void initializeBoard(Board *board) {
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            board->cells[i][j] = ' ';
        }
    }
}

// Function to print the current state of the board with row and column numbers
void printBoard(Board *board) {
    printf("\n   "); // Initial spacing for row numbers

    // Print column numbers at the top
    for (int col = 1; col <= board->size; col++) {
        printf(" %d  ", col);
    }
    printf("\n");

    for (int i = 0; i < board->size; i++) {
        printf(" %d ", i + 1); // Print row number on the left
        for (int j = 0; j < board->size; j++) {
            printf(" %c ", board->cells[i][j]);
            if (j < board->size - 1) printf("|");
        }
        printf("\n");

        // Print horizontal separators between rows
        if (i < board->size - 1) {
            printf("   "); // Align separators with column numbers
            for (int k = 0; k < board->size; k++) {
                printf("---");
                if (k < board->size - 1) printf("|");
            }
            printf("\n");
        }
    }
    printf("\n");
}

// Function to free the allocated resources
void freeBoard(Board *board) {
    for (int i = 0; i < board->size; i++) {
        free(board->cells[i]);
    }
    free(board->cells);
}

// Function to check if the board is full
int isBoardFull(Board *board) {
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            if (board->cells[i][j] == ' ') return 0;
        }
    }
    return 1;
}
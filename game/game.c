#include "game.h"

#include <limits.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

// Function to check if a player has won
int checkWin(Board *board, char player) {
    for (int i = 0; i < board->size; i++) {
        int rowWin = 1, colWin = 1;
        for (int j = 0; j < board->size; j++) {
            if (board->cells[i][j] != player) rowWin = 0;
            if (board->cells[j][i] != player) colWin = 0;
        }
        if (rowWin || colWin) return 1;
    }
    int diag1Win = 1, diag2Win = 1;
    for (int i = 0; i < board->size; i++) {
        if (board->cells[i][i] != player) diag1Win = 0;
        if (board->cells[i][board->size - 1 - i] != player) diag2Win = 0;
    }
    return diag1Win || diag2Win;
}

// Function to print the winner of the game
void printWinner(const char winner) {
    if (winner == 'X') {
        printf("X wins!\n");
    } else if (winner == 'O') {
        printf("O wins!\n");
    } else {
        printf("It's a draw!\n");
    }
}

// Helper function to score a line based on some rules
int evaluateLine(Board *board, int row, int column, int rowDirection, int columnDirection, char OPlayer, char XPlayer) {
    int OPlayerCount = 0;
    int XPlayerCount = 0;

    for (int i = 0; i < board->size; i++) {
        char cell = board->cells[row][column];
        if (cell == OPlayer) {
            OPlayerCount++;
        } else if (cell == XPlayer) {
            XPlayerCount++;
        }
        row += rowDirection;
        column += columnDirection;
    }

    if (OPlayerCount > 0 && XPlayerCount > 0) {
        return 0;
    }

    if (OPlayerCount > 0) {
        return (int)pow(10, OPlayerCount - 1);
    }

    if (XPlayerCount > 0) {
        return -(int)pow(10, XPlayerCount - 1);
    }

    return 0;
}

int staticEvaluation(Board *board, char OPlayer, char XPlayer) {
    int totalScore = 0;

    // Rows
    for (int i = 0; i < board->size; i++) {
        totalScore += evaluateLine(board, i, 0, 0, 1, OPlayer, XPlayer);
    }

    // Columns
    for (int j = 0; j < board->size; j++) {
        totalScore += evaluateLine(board, 0, j, 1, 0, OPlayer, XPlayer);
    }

    // Principal diagonal
    totalScore += evaluateLine(board, 0, 0, 1, 1, OPlayer, XPlayer);

    // Secondary diagonal
    totalScore += evaluateLine(board, 0, board->size - 1, 1, -1, OPlayer, XPlayer);

    return totalScore;
}

// Minimax algorithm with alpha-beta pruning and depth limit
int minimax(Board *board, int depth, int isMaximizing, int alpha, int beta, int maxDepth) {
    if (checkWin(board, 'O')) return 10000000 - depth;
    if (checkWin(board, 'X')) return depth - 10000000;
    if (isBoardFull(board) || depth == maxDepth) return staticEvaluation(board, 'O', 'X');

    if (isMaximizing) {
        int bestScore = INT_MIN;
        for (int i = 0; i < board->size; i++) {
            for (int j = 0; j < board->size; j++) {
                if (board->cells[i][j] == ' ') {
                    board->cells[i][j] = 'O';
                    int score = minimax(board, depth + 1, 0, alpha, beta, maxDepth);
                    board->cells[i][j] = ' ';
                    bestScore = score > bestScore ? score : bestScore;
                    alpha = alpha > bestScore ? alpha : bestScore;
                    if (beta <= alpha) return bestScore;
                }
            }
        }
        return bestScore;
    }

    int bestScore = INT_MAX;
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            if (board->cells[i][j] == ' ') {
                board->cells[i][j] = 'X';
                int score = minimax(board, depth + 1, 1, alpha, beta, maxDepth);
                board->cells[i][j] = ' ';
                bestScore = score < bestScore ? score : bestScore;
                beta = beta < bestScore ? beta : bestScore;
                if (beta <= alpha) return bestScore;
            }
        }
    }
    return bestScore;
}

// Function for the player to make a move using row and column input
void playerMove(Board *board) {
    int row, col;
    while (1) {
        printf("Enter your move (row number from 1 to %d): ", board->size);
        if (scanf("%d", &row) != 1 || row < 1 || row > board->size) {
            printf("Invalid input. Please enter a valid row number.\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }

        printf("Enter your move (column number from 1 to %d): ", board->size);
        if (scanf("%d", &col) != 1 || col < 1 || col > board->size) {
            printf("Invalid input. Please enter a valid column number.\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }

        row--; // Adjust for 0-indexed array
        col--;
        if (board->cells[row][col] == ' ') {
            board->cells[row][col] = 'X';
            break;
        }
        printf("Cell already taken. Try again.\n");
    }
}

// Function for the computer to make a move
double computerMove(Board *board, char currentMarker, int isMaximizingPlayer, int maxDepth) {
    double startTime = omp_get_wtime();
    int bestScore = isMaximizingPlayer ? INT_MIN : INT_MAX;
    int moveRow = -1, moveCol = -1;

    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            if (board->cells[i][j] == ' ') {
                board->cells[i][j] = currentMarker;
                int score = minimax(board, 0, !isMaximizingPlayer, INT_MIN, INT_MAX, maxDepth);
                board->cells[i][j] = ' ';
                if (isMaximizingPlayer) {
                    if (score > bestScore) {
                        bestScore = score;
                        moveRow = i;
                        moveCol = j;
                    }
                } else {
                    if (score < bestScore) {
                        bestScore = score;
                        moveRow = i;
                        moveCol = j;
                    }
                }
            }
        }
    }
    board->cells[moveRow][moveCol] = currentMarker;
    double endTime = omp_get_wtime();
    return endTime - startTime;
}

// Parallel function for the computer to make a move - parallel v1
double computerMoveParallelV1(Board *board, char currentMarker, int isMaximizingPlayer, int maxDepth, int numberOfThreads) {
    double startTime = omp_get_wtime();

    typedef struct { int r, c; } Move;

    Move* possibleMoves = (Move*)malloc(board->size * board->size * sizeof(Move));
    int* scores = (int*)malloc(board->size * board->size * sizeof(int));
    int totalPossibleMoves = 0;

    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            if (board->cells[i][j] == ' ') {
                possibleMoves[totalPossibleMoves].r = i;
                possibleMoves[totalPossibleMoves].c = j;
                totalPossibleMoves++;
            }
        }
    }

    #pragma omp parallel for num_threads(numberOfThreads) default(none) shared(board, isMaximizingPlayer, maxDepth, possibleMoves, scores, totalPossibleMoves) firstprivate(currentMarker) schedule(dynamic)
    for (int k = 0; k < totalPossibleMoves; k++) {
        int i = possibleMoves[k].r;
        int j = possibleMoves[k].c;

        Board localBoard = copyBoard(board);
        localBoard.cells[i][j] = currentMarker;
        int score = minimax(&localBoard, 0, !isMaximizingPlayer, INT_MIN, INT_MAX, maxDepth);
        freeBoard(&localBoard);

        scores[k] = score;
    }

    int bestScore = isMaximizingPlayer ? INT_MIN : INT_MAX;
    int bestMoveIndex = 0;

    for (int k = 0; k < totalPossibleMoves; k++) {
        if (isMaximizingPlayer) {
            if (scores[k] > bestScore) {
                bestScore = scores[k];
                bestMoveIndex = k;
            }
        } else {
            if (scores[k] < bestScore) {
                bestScore = scores[k];
                bestMoveIndex = k;
            }
        }
    }

    board->cells[possibleMoves[bestMoveIndex].r][possibleMoves[bestMoveIndex].c] = currentMarker;

    free(possibleMoves);
    free(scores);
    double endTime = omp_get_wtime();
    return endTime - startTime;
}

// Parallel function for the computer to make a move - parallel v2
double computerMoveParallelV2(Board *board, char currentMarker, int isMaximizingPlayer, int maxDepth, int numberOfThreads) {
    double startTime = omp_get_wtime();
    typedef struct { int r, c; } Move;

    Move* possibleMoves = (Move*)malloc(board->size * board->size * sizeof(Move));
    int* scores = (int*)malloc(board->size * board->size * sizeof(int));
    int totalPossibleMoves = 0;

    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            if (board->cells[i][j] == ' ') {
                possibleMoves[totalPossibleMoves].r = i;
                possibleMoves[totalPossibleMoves].c = j;
                totalPossibleMoves++;
            }
        }
    }

    #pragma omp parallel num_threads(numberOfThreads) default(none) shared(board, isMaximizingPlayer, maxDepth, possibleMoves, scores, totalPossibleMoves) firstprivate(currentMarker)
    #pragma omp single
    for (int k = 0; k < totalPossibleMoves; k++) {
        #pragma omp task default(none) shared(possibleMoves, scores, board, isMaximizingPlayer, currentMarker, maxDepth, k)
        {
            int i = possibleMoves[k].r;
            int j = possibleMoves[k].c;

            Board localBoard = copyBoard(board);

            localBoard.cells[i][j] = currentMarker;

            scores[k] = minimax(&localBoard, 0, !isMaximizingPlayer, INT_MIN, INT_MAX, maxDepth);

            freeBoard(&localBoard);
        }
    }

    int bestScore = isMaximizingPlayer ? INT_MIN : INT_MAX;
    int bestMoveIndex = 0;

    for (int k = 0; k < totalPossibleMoves; k++) {
        if (isMaximizingPlayer) {
            if (scores[k] > bestScore) {
                bestScore = scores[k];
                bestMoveIndex = k;
            }
        } else {
            if (scores[k] < bestScore) {
                bestScore = scores[k];
                bestMoveIndex = k;
            }
        }
    }

    board->cells[possibleMoves[bestMoveIndex].r][possibleMoves[bestMoveIndex].c] = currentMarker;

    free(possibleMoves);
    free(scores);
    double endTime = omp_get_wtime();
    return endTime - startTime;
}

double makeComputerMove(Board *board, char marker, int isMaximizing, int maxDepth, int algorithm, int numThreads) {
    switch (algorithm) {
        case 1:
            return computerMove(board, marker, isMaximizing, maxDepth);
        case 2:
            return computerMoveParallelV1(board, marker, isMaximizing, maxDepth, numThreads);
        case 3:
            return computerMoveParallelV2(board, marker, isMaximizing, maxDepth, numThreads);
        default:
            printf("Error: Invalid algorithm choice.\n");
            return 0.0;
    }
}

void runPlayerVsComputer(Board *board, int maxDepth, int algorithm, int numThreads) {
    char winner = ' ';

    printf("\nYou are 'X'. The Computer is 'O'.\n");

    while (1) {
        printBoard(board);
        playerMove(board);

        if (checkWin(board, 'X')) {
            winner = 'X';
            break;
        }
        if (isBoardFull(board)) break;

        printBoard(board);
        double moveTime = makeComputerMove(board, 'O', 0, maxDepth, algorithm, numThreads);
        printf("Computer move took %.4f seconds.\n", moveTime);

        if (checkWin(board, 'O')) {
            winner = 'O';
            break;
        }
        if (isBoardFull(board)) break;
    }

    printBoard(board);
    printWinner(winner);
}

double runComputerVsComputer(Board *board, int maxDepth, int algorithm, int numThreads, int debugMode) {
    char winner = ' ';
    int moves = 0;
    double totalTime = 0.0;

    if (debugMode) printf("DEBUG mode: Printing all boards.\n");

    while (1) {
        double moveTime;

        if (moves == 0) {
            board->cells[0][0] = 'X';
            moveTime = 0.0;
        } else {
            moveTime = makeComputerMove(board, 'X', 1, maxDepth, algorithm, numThreads);
        }

        totalTime += moveTime;
        moves++;

        if (debugMode) {
            printf("Move %d (X) took %.4f seconds.\n", moves, moveTime);
            printBoard(board);
        }

        if (checkWin(board, 'X')) {
            winner = 'X';
            break;
        }
        if (isBoardFull(board)) break;

        moveTime = makeComputerMove(board, 'O', 0, maxDepth, algorithm, numThreads);
        totalTime += moveTime;
        moves++;

        if (debugMode) {
            printf("Move %d (O) took %.4f seconds.\n", moves, moveTime);
            printBoard(board);
        }

        if (checkWin(board, 'O')) {
            winner = 'O';
            break;
        }
        if (isBoardFull(board)) break;
    }

    if (debugMode) {
        printBoard(board);
    }

    if (debugMode) {
        printWinner(winner);
        printf("--- CvsC Performance (%d moves) ---\n", moves);
        printf("Total computation time: %.4f seconds.\n", totalTime);
    }

    // We subtract 1 from moves because the first move was deterministic
    if (moves > 1) {
        double avgMoveTime = totalTime / (moves - 1);
        if (debugMode) printf("Average move time: %.4f seconds.\n", avgMoveTime);
        return avgMoveTime;
    }

    return 0.0;
}
#ifndef GENERALIZEDTICTACTOE_GAME_H
#define GENERALIZEDTICTACTOE_GAME_H

#include "board.h"

int checkWin(Board *board, char player);

void printWinner(char winner);

int evaluateLine(Board *board, int row, int column, int rowDirection, int columnDirection, char OPlayer, char XPlayer);

int staticEvaluation(Board *board, char OPlayer, char XPlayer);

int minimax(Board *board, int depth, int isMaximizing, int alpha, int beta, int maxDepth);

void playerMove(Board *board);

double computerMove(Board *board, char currentMarker, int isMaximizingPlayer, int maxDepth);

double computerMoveParallelV1(Board *board, char currentMarker, int isMaximizingPlayer, int maxDepth, int numberOfThreads);

double computerMoveParallelV2(Board *board, char currentMarker, int isMaximizingPlayer, int maxDepth, int numberOfThreads);

double makeComputerMove(Board *board, char marker, int isMaximizing, int maxDepth, int algorithm, int numThreads);

void runPlayerVsComputer(Board *board, int maxDepth, int algorithm, int numThreads);

double runComputerVsComputer(Board *board, int maxDepth, int algorithm, int numThreads, int debugMode);

#endif //GENERALIZEDTICTACTOE_GAME_H
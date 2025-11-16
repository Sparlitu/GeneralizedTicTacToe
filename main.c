#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "board/board.h"
#include "game/game.h"

int getIntInput(const char *prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1 && value >= min && value <= max) {
            while (getchar() != '\n');
            return value;
        }
        printf("Invalid input. Please enter a number between %d and %d.\n", min, max);
        while (getchar() != '\n');
    }
}

void runPerformanceTest(int size, int maxDepth, int algorithm, int numThreads, int gameMode, int debugMode) {
    Board board = createBoard(size);
    initializeBoard(&board);

    if (gameMode == 2 && debugMode == 0) {
        double avgMoveTime = runComputerVsComputer(&board, maxDepth, algorithm, numThreads, 0);

        // Print the CSV data line
        // Format: N, Depth, Algorithm, Threads, AvgMoveTime
        printf("%d,%d,%d,%d,%.6f\n", size, maxDepth, algorithm, numThreads, avgMoveTime);

    } else {
        printf("Non-interactive mode only supports CvsC in PERFORMANCE mode.\n");
    }

    freeBoard(&board);
}

void runInteractiveMode() {

    int size = getIntInput("Enter the size of the board (N) [3-9]: ", 3, 9);

    int maxDepth = getIntInput("Enter max depth [4-10]: ", 4, 10);

    printf("\nSelect Game Mode:\n");
    printf(" (1) Player vs. Computer\n");
    printf(" (2) Computer vs. Computer\n");
    int gameMode = getIntInput("Choice [1-2]: ", 1, 2);

    printf("\nSelect Algorithm:\n");
    printf(" (1) Serial (1 Thread)\n");
    printf(" (2) Parallel V1 (omp parallel for)\n");
    printf(" (3) Parallel V2 (omp task)\n");
    int algorithm = getIntInput("Choice [1-3]: ", 1, 3);

    int numThreads = 1;
    if (algorithm > 1) {
        printf("\nSelect Number of Threads:\n");
        printf(" (1) 2 Threads\n");
        printf(" (2) 4 Threads\n");
        printf(" (3) 8 Threads\n");
        int threadChoice = getIntInput("Choice [1-3]: ", 1, 3);
        switch (threadChoice) {
            case 1: numThreads = 2; break;
            case 2: numThreads = 4; break;
            case 3: numThreads = 8; break;
        }
    }
    printf("Using %d thread(s).\n", numThreads);

    int debugMode = 1;
    if (gameMode == 2) {
        printf("\nSelect CvsC Mode:\n");
        printf(" (1) DEBUG (Print all moves and boards)\n");
        printf(" (2) PERFORMANCE (Run silently for speed)\n");
        debugMode = getIntInput("Choice [1-2]: ", 1, 2);
        debugMode = debugMode == 1;
    }

    Board board = createBoard(size);
    initializeBoard(&board);
    printf("\nStarting %dx%d game. Max Depth: %d. Algorithm: %d. Threads: %d.\n",
           size, size, maxDepth, algorithm, numThreads);

    if (gameMode == 1) {
        runPlayerVsComputer(&board, maxDepth, algorithm, numThreads);
    } else {
        double totalGameTime = runComputerVsComputer(&board, maxDepth, algorithm, numThreads, debugMode);

        if (!debugMode) {
            printf("----------------------------------------\n");
            printf("Total Wall-Clock Time: %.4f seconds\n", totalGameTime);
            printf("----------------------------------------\n");
        }
    }

    freeBoard(&board);
}


// Main function to run the game
int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed the random number generator

    if (argc == 7) {
        // ./GeneralizedTicTacToe <N> <Depth> <Algorithm> <Threads> <GameMode> <DebugMode>
        int size       = atoi(argv[1]);
        int maxDepth   = atoi(argv[2]);
        int algorithm  = atoi(argv[3]);
        int numThreads = atoi(argv[4]);
        int gameMode   = atoi(argv[5]);
        int debugMode  = atoi(argv[6]);

        runPerformanceTest(size, maxDepth, algorithm, numThreads, gameMode, debugMode);
    } else {
        runInteractiveMode();
    }

    return 0;
}
#!/bin/bash

EXECUTABLE="./cmake-build-debug/GeneralizedTicTacToe"

OUTPUT_FILE="results.csv"

BOARD_SIZES=(5 6)
DEPTHS=(5 6)
THREADS=(2 4 8)
GAME_MODE=2
DEBUG_MODE=0

# Create the CSV file header
echo "N,Depth,Algorithm,Threads,AvgMoveTime" > $OUTPUT_FILE

for N in "${BOARD_SIZES[@]}"; do
  for DEPTH in "${DEPTHS[@]}"; do

    # Test Serial (Algorithm 1)
    ALGO=1
    NUM_THREADS=1
    echo "Testing: N=$N, Depth=$DEPTH, Algo=Serial, Threads=1"
    # Run the test and append the CSV output to our file
    $EXECUTABLE "$N" "$DEPTH" $ALGO $NUM_THREADS $GAME_MODE $DEBUG_MODE >> $OUTPUT_FILE

    # Test Parallel V1 (Algorithm 2)
    ALGO=2
    for T in "${THREADS[@]}"; do
      echo "Testing: N=$N, Depth=$DEPTH, Algo=V1, Threads=$T"
      $EXECUTABLE "$N" "$DEPTH" $ALGO "$T" $GAME_MODE $DEBUG_MODE >> $OUTPUT_FILE
    done

    # Test Parallel V2 (Algorithm 3)
    ALGO=3
    for T in "${THREADS[@]}"; do
      echo "Testing: N=$N, Depth=$DEPTH, Algo=V2, Threads=$T"
      $EXECUTABLE "$N" "$DEPTH" $ALGO "$T" $GAME_MODE $DEBUG_MODE >> $OUTPUT_FILE
    done

  done
done

echo "---"
echo "Experiment run complete. Results saved in $OUTPUT_FILE."
cat $OUTPUT_FILE
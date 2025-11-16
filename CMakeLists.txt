cmake_minimum_required(VERSION 4.0)
project(GeneralizedTicTacToe C)

set(CMAKE_C_STANDARD 11)

include_directories(board game)

add_executable(GeneralizedTicTacToe main.c
        board/board.c
        board/board.h
        game/game.c
        game/game.h)

find_package(OpenMP REQUIRED)
if(OpenMP_C_FOUND)
    target_link_libraries(GeneralizedTicTacToe PUBLIC OpenMP::OpenMP_C)
endif()

target_link_libraries(GeneralizedTicTacToe PUBLIC m)
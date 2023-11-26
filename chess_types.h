// chess_types.h

#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

#include "smallchesslib.h"

// Define the chessgame data type
typedef struct {
    SCL_Board board;   // Current state of the chessboard
    SCL_Record record; // Record of the moves made in the game
    // Add more fields if necessary
} chessgame;

// Define the chessboard data type
typedef struct {
    SCL_Board board; // Current state of the chessboard
    // Add more fields if necessary
} chessboard;

// Function declarations
void parseSANToChessgame(const char* san, chessgame* game);
void chessgameToSAN(const chessgame* game, char* sanOutput);
void parseFENToChessboard(const char* fen, chessboard* board);
void chessboardToFEN(const chessboard* board, char* fenOutput);

#endif // CHESS_TYPES_H

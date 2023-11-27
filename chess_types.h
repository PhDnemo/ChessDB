// chess_types.h

#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

#include "smallchesslib.h"

#define FEN_MAX_LENGTH 100
#define SAN_MAX_LENGTH 2000

typedef struct {
    char fen[FEN_MAX_LENGTH]; // FEN representation of the chessboard
    // Additional fields can be added as required
} chessboard;

typedef struct {
    char san[SAN_MAX_LENGTH]; // SAN representation of the chess game
    chessboard currentBoard;   // Current state of the chessboard
    // Additional fields can be added as required
} chessgame;


// Function declarations
void parseSANToChessgame(const char* san, chessgame* game);
void appendCharToSANString(char c, char **sanOutput) 
void chessgameToSAN(const chessgame* game, char* sanOutput);
void parseFENToChessboard(const char* fen, chessboard* board);
void chessboardToFEN(const chessboard* board, char* fenOutput);

#endif // CHESS_TYPES_H

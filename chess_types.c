// chess_types.c

#include "smallchesslib.h"
#include "chess_types.h"

// Assumption: Each move in the SAN string is given in a simple format, such as "e4", "Nf3", etc.
void parseSANToChessgame(const char* san, chessgame* game) {
}


void chessgameToSAN(const chessgame* game, char* sanOutput) {
}

void parseFENToChessboard(const char* fen, chessboard* board) {
    if (SCL_boardFromFEN(board->board, fen) == 0) {
        // Error handling: FEN string could not be parsed
        printf("Error: Invalid FEN string provided.\n");
    }
}

void chessboardToFEN(const chessboard* board, char* fenOutput) {
    if (SCL_boardToFEN(board->board, fenOutput) == 0) {
        // Error handling: FEN conversion failed
        printf("Error: Unable to convert chessboard to FEN notation.\n");
    }
}
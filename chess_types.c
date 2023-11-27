// chess_types.c

#include "smallchesslib.h"
#include "chess_types.h"

// Assumption: Each move in the SAN string is given in a simple format, such as "e4", "Nf3", etc.
void parseSANToChessgame(const char* san, chessgame* game) {
    if (SCL_recordFromPGN(game->record, san) == 0) {
        // Error handling: PGN string could not be parsed
        printf("Error: Invalid PGN string provided.\n");
    }
}

// Helper Function
void appendCharToSANString(char c, char **sanOutput) {
    *(*sanOutput) = c;  // Set the current character
    (*sanOutput)++;     // Move the pointer to the next position
}

void chessgameToSAN(const chessgame* game, char* sanOutput) {
    char* outputPointer = sanOutput;  // Pointer to the current position in the output string

    // Function pointer to appendCharToSANString with the address of outputPointer
    SCL_PutCharFunction putCharFunc = (SCL_PutCharFunction) appendCharToSANString;

    // Call SCL_printPGN with the game record, the custom putCharFunc, and initial board state
    SCL_printPGN(game->record, putCharFunc, &outputPointer, game->initialState);

    *outputPointer = '\0';  // Null-terminate the output string
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
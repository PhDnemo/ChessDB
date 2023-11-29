// chess_types.c
#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
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
/*****************************************************************************/

PG_FUNCTION_INFO_V1(SCL_board_in);

Datum
SCL_board_in(PG_FUNCTION_ARGS)
{
    char *fen_str = PG_GETARG_CSTRING(0);

    // tranfer FEN to SCL_Board
    SCL_Board result_board = SCL_boardFromFEN(fen_str);

    // throw error
    if (result_board == INVALID_SOME_VALUE)
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for SCL_Board: \"%s\"", fen_str)));

    PG_RETURN_SCL_BOARD(result_board);
}

PG_FUNCTION_INFO_V1(SCL_board_out);

Datum
SCL_board_out(PG_FUNCTION_ARGS)
{
    SCL_Board board = PG_GETARG_SCL_BOARD(0);

    // tranfer SCL_Board to FEN
    char *fen_str = SCL_boardToFEN(board);

    // throw error
    if (fen_str == NULL)
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("failed to convert SCL_Board to FEN")));

    PG_RETURN_CSTRING(fen_str);
}

PG_FUNCTION_INFO_V1(SCL_board_recv);
Datum
SCL_board_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);

    SCL_Board *board = (SCL_Board *) palloc0(sizeof(SCL_Board));

    // Read data from a binary stream to SCL_Board
    if (!SCL_boardFromBinary(buf, board))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
                 errmsg("invalid binary data for SCL_Board")));
    }

    PG_RETURN_POINTER(board);
}

PG_FUNCTION_INFO_V1(SCL_board_send);
Datum
SCL_board_send(PG_FUNCTION_ARGS)
{
    SCL_Board *board = (SCL_Board *) PG_GETARG_POINTER(0);
    StringInfoData buf;
    initStringInfo(&buf);
    if (!SCL_boardToBinary(&buf, board))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
                 errmsg("failed to convert SCL_Board to binary")));
    }
    PG_RETURN_BYTEA_P(buf.data);
}

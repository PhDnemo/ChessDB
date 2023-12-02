// Include PostgreSQL headers
#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"

// Include standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Include custom headers
#include "smallchesslib.h"

// PostgreSQL module magic
PG_MODULE_MAGIC;

#define MY_BOARD_START_STATE "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define SAN_OUTPUT_SIZE 1024
#define FEN_OUTPUT_SIZE 1024

static void  putCharacter(char c)
{
  putchar(c);
}
// #define SCL_FEN_MAX_LENGTH 1024

/*******************************************************************
 * Input and Output functions
 * For FEN -- Chessboard
 ******************************************************************/
PG_FUNCTION_INFO_V1(SCL_board_in);
Datum SCL_board_in(PG_FUNCTION_ARGS) {
    char *fen_str = PG_GETARG_CSTRING(0);

    // Allocate memory for SCL_Board
    SCL_Board *result_board = palloc0(sizeof(SCL_Board));

    // Transfer FEN to SCL_Board
    if (!SCL_boardFromFEN(*result_board, fen_str)) {
        // If conversion fails, throw an error
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for FEN: \"%s\"", fen_str)));
    }

    // Return the SCL_Board
    PG_RETURN_POINTER(result_board);
}

PG_FUNCTION_INFO_V1(SCL_board_out);
Datum SCL_board_out(PG_FUNCTION_ARGS) {
    SCL_Board *board = (SCL_Board *) PG_GETARG_POINTER(0);

    // Allocate sufficient memory for the FEN string
    char fen_str[SCL_FEN_MAX_LENGTH];

    // Convert the board state to a FEN string using the SCL_boardToFEN function
    if (!SCL_boardToFEN(*board, fen_str)) {
        // If conversion fails, throw an error
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("failed to convert SCL_Board to FEN")));
    }
    PG_RETURN_TEXT_P(fen_str);
}

/*******************************************************************
 * Input and Output functions
 * For SAN -- Chessgame
**************************************************************/

PG_FUNCTION_INFO_V1(san_in); // Input
Datum san_in(PG_FUNCTION_ARGS) {
    // Retrieve the input SAN string
    char *san_str = PG_GETARG_CSTRING(0);
    text *record = cstring_to_text(san_str);
    PG_RETURN_TEXT_P(record); // Return the text data type
}

PG_FUNCTION_INFO_V1(san_out);
Datum san_out(PG_FUNCTION_ARGS) {

  // Retrieve the input SCL_Record object
  SCL_Record *record = (SCL_Record *) PG_GETARG_POINTER(0);
  char *san_str = text_to_cstring(record); // Convert text to C string
  PG_RETURN_CSTRING(san_str);

}

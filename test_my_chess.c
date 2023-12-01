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
        elog(ERROR, "failed to convert SCL_Board to FEN");
    }

    // Print some debugging information
    elog(NOTICE, "FEN string: %s", fen_str);

    // Convert the FEN string to PostgreSQL's text data type
    // text *output_text = cstring_to_text(fen_str);

    // Return the text
    PG_RETURN_TEXT_P(fen_str);
}

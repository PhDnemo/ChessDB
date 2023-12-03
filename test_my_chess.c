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

/*******************************************************************
 * B Tree Index Related updated 12/03 12:00
 * For SAN
**************************************************************/
PG_FUNCTION_INFO_V1(chessgame_cmp);
Datum chessgame_cmp(PG_FUNCTION_ARGS) {
    // Extract the two input SAN strings
    text *san_text_1 = PG_GETARG_TEXT_PP(0);
    text *san_text_2 = PG_GETARG_TEXT_PP(1);
    // Convert the text to C strings
    char *san_str_1 = text_to_cstring(san_text_1);
    char *san_str_2 = text_to_cstring(san_text_2);
    // Compare the two strings using strcmp
    // If result < 0: the first key is less than the second key.
    // If result = 0: the keys are equal.
    // If result > 0: the first key is greater than the second key.
    int result = strcmp(san_str_1, san_str_2);
    // Free the allocated memory
    pfree(san_str_1);
    pfree(san_str_2);
    // Return the comparison result
    PG_RETURN_INT32(result);
}


PG_FUNCTION_INFO_V1(chessgame_eq);
Datum chessgame_eq(PG_FUNCTION_ARGS) {
    // Call chessgame_cmp and check for equality
    int32 result = DatumGetInt32(DirectFunctionCall2(chessgame_cmp, PG_GETARG_DATUM(0), PG_GETARG_DATUM(1)));
    PG_RETURN_BOOL(result == 0);
}

PG_FUNCTION_INFO_V1(chessgame_ne);
Datum chessgame_ne(PG_FUNCTION_ARGS) {
    // Call chessgame_cmp and check for inequality
    int32 result = DatumGetInt32(DirectFunctionCall2(chessgame_cmp, PG_GETARG_DATUM(0), PG_GETARG_DATUM(1)));
    PG_RETURN_BOOL(result != 0);
}

PG_FUNCTION_INFO_V1(chessgame_lt);
Datum chessgame_lt(PG_FUNCTION_ARGS) {
    // Call chessgame_cmp and check for less than
    int32 result = DatumGetInt32(DirectFunctionCall2(chessgame_cmp, PG_GETARG_DATUM(0), PG_GETARG_DATUM(1)));
    PG_RETURN_BOOL(result < 0);
}

PG_FUNCTION_INFO_V1(chessgame_le);
Datum chessgame_le(PG_FUNCTION_ARGS) {
    // Call chessgame_cmp and check for less than or equal to
    int32 result = DatumGetInt32(DirectFunctionCall2(chessgame_cmp, PG_GETARG_DATUM(0), PG_GETARG_DATUM(1)));
    PG_RETURN_BOOL(result <= 0);
}

PG_FUNCTION_INFO_V1(chessgame_gt);
Datum chessgame_gt(PG_FUNCTION_ARGS) {
    // Call chessgame_cmp and check for greater than
    int32 result = DatumGetInt32(DirectFunctionCall2(chessgame_cmp, PG_GETARG_DATUM(0), PG_GETARG_DATUM(1)));
    PG_RETURN_BOOL(result > 0);
}

PG_FUNCTION_INFO_V1(chessgame_ge);
Datum chessgame_ge(PG_FUNCTION_ARGS) {
    // Call chessgame_cmp and check for greater than or equal to
    int32 result = DatumGetInt32(DirectFunctionCall2(chessgame_cmp, PG_GETARG_DATUM(0), PG_GETARG_DATUM(1)));
    PG_RETURN_BOOL(result >= 0);
}

//Function to return the board state at a given half-move

PG_FUNCTION_INFO_V1(getBoard);
Datum getBoard(PG_FUNCTION_ARGS) {

    //SCL_Record *record = (SCL_Record *) PG_GETARG_POINTER(0);
    char* fenstr = (char*)malloc(70 * sizeof(char));//allocation of size 70 char(FEN notation does not exceed 70 character)
    strcpy(fenstr,"");
    text *san_text = PG_GETARG_TEXT_P(0);
    char *san_str = text_to_cstring(san_text);

    uint16_t halfMoveCount = PG_GETARG_UINT16(1);

    SCL_Record record;
    SCL_recordFromPGN(record,san_str);
    SCL_Board* board = (SCL_Board*)malloc(sizeof(SCL_Board));
    SCL_boardInit(board);
    uint16_t recordLength = SCL_recordLength(record);
    //SCL_recordFromPGN(record,halfMoveCount);

    //char *recordString1 = text_to_cstring(record);

    //elog(NOTICE, "recordFromPGN: %s", recordString1);

    SCL_recordApply(record, board, halfMoveCount);


    int t = SCL_boardToFEN(board, fenstr);//Get the FEN notation as a string of the board state and returns the size of the string


    //elog(NOTICE, "Fullrecord: %s", record);
    //elog(NOTICE, "Fullrecord: %*H", SAN_OUTPUT_SIZE, record);
    //打印有问题，待看
    //elog(NOTICE, "Fullrecord: %.*s", (int)SAN_OUTPUT_SIZE, record);

    //elog(NOTICE, "halfMoveCount: %u", halfMoveCount);
    //elog(NOTICE, "recordLength: %u", recordLength);

    return fenstr;
}

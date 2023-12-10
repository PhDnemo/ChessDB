// Include PostgreSQL headers
#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "utils/array.h"
#include "utils/fmgroids.h"
#include "access/gin.h"
#include "funcapi.h"


// Include standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Include smallchesslib headers
#include "smallchesslib.h"

// PostgreSQL module magic
PG_MODULE_MAGIC;

char* getBoard_internal(text *san_text, int halfMoves);

bool hasOpening_internal(text *san_text1, text *san_text2);

char* get_first_moves_internal(const char *chessgame, int halfMoves);

bool hasBoard_internal(text *san_text, SCL_Board *targetBoard, int halfMoves);

//bool compare_internal(char *state1, char *state2);

bool errorNumCheck(text *san_text, int halfMoves);

/*******************************************************************
 * Input and Output functions
 * For FEN -- chessboard
 ******************************************************************/
PG_FUNCTION_INFO_V1(chessboard_in);
Datum chessboard_in(PG_FUNCTION_ARGS) {
    char *fen_str = PG_GETARG_CSTRING(0);
    // Allocate memory for SCL_Board
    SCL_Board *result_board = palloc0(sizeof(SCL_Board));

    // Transfer FEN to SCL_Board
    if (!SCL_boardFromFEN(*result_board, fen_str)) //highlight
    {
        // If conversion fails, throw an error
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for FEN: \"%s\"", fen_str)));
    }
    // Return the chessboard
    PG_RETURN_POINTER(result_board);
}

PG_FUNCTION_INFO_V1(chessboard_out);
Datum chessboard_out(PG_FUNCTION_ARGS) {
    SCL_Board *board = (SCL_Board *) PG_GETARG_POINTER(0);

    char fen_str[SCL_FEN_MAX_LENGTH];

    // Convert the board state to a FEN string using the SCL_boardToFEN function
    if (!SCL_boardToFEN(*board, fen_str)) //highlight
    {
        // If conversion fails, throw an error
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("failed to convert SCL_Board to FEN")));
    }
    // Return the fen_str
    PG_RETURN_CSTRING(fen_str);
}

/*******************************************************************
 * Input and Output functions
 * For SAN -- chessgame
**************************************************************/

PG_FUNCTION_INFO_V1(chessgame_in);
Datum chessgame_in(PG_FUNCTION_ARGS) {
    // Retrieve the input SAN string
    char *san_str = PG_GETARG_CSTRING(0);

    text *record = cstring_to_text(san_str); //highlight

    PG_RETURN_TEXT_P(record);
}

PG_FUNCTION_INFO_V1(chessgame_out);
Datum chessgame_out(PG_FUNCTION_ARGS) {

  text *record_txt = PG_GETARG_TEXT_P(0);

  char *san_str = text_to_cstring(record_txt); //highlight

  PG_RETURN_CSTRING(san_str);

}

//Check whether the number of entered halfMoves is negative or exceeds the actual number of halfMovesin the game
bool errorNumCheck(text *san_text, int halfMoves)
{
      char *chessgame_str = text_to_cstring(san_text);

      SCL_Record record;

      SCL_recordFromPGN(record,chessgame_str);

      if(halfMoves<0)
      {
        elog(ERROR, "Wrong halfMoves input, the Number should should not be negative");
        return false;
      }

      if(halfMoves>SCL_recordLength(record))
      {
        elog(ERROR, "Wrong halfMoves input, the Number should should not be greater than:%d", SCL_recordLength(record));
        return false;
      }

      return true;
}

//  Return the board state at a given half-move
char* getBoard_internal(text* san_text, int halfMoves){

    char *san_str = text_to_cstring(san_text);

    SCL_Record record;

    SCL_Board *board;

    char *fenstr;

    SCL_recordFromPGN(record,san_str); //highlight

    board = palloc0(sizeof(SCL_Board));

    SCL_recordApply(record, *board, halfMoves); //highlight

    fenstr = palloc(70 * sizeof(char));//allocation of size 70 char(FEN notation does not exceed 70 character)

    strcpy(fenstr,"");

    SCL_boardToFEN(*board, fenstr);//highlight

    return fenstr;
}

//Function to return the board state at a given halfMoves
PG_FUNCTION_INFO_V1(getBoard);
Datum getBoard(PG_FUNCTION_ARGS) {

    text *san_text = PG_GETARG_TEXT_PP(0);

    uint16_t halfMoves = PG_GETARG_UINT16(1);

    char *result = NULL;

    // if input halfMoves greater than the recoardlength, throw ERROR
    if(errorNumCheck(san_text,halfMoves) == true) //highlight
    {
        result = getBoard_internal(san_text,halfMoves); //highlight
    }

    PG_RETURN_CSTRING(result);
}

//Returns true if the first chess game starts with the exact same set of moves as the second chess game.
bool hasOpening_internal(text *san_text1, text *san_text2)

{

    char *san_str1 = text_to_cstring(san_text1);

    char *san_str2 = text_to_cstring(san_text2);
    //Assume that the input is in the correct san format
    if(strlen(san_str1)<strlen(san_str2)) //highlight
    {
      return false;
    }
    return strncmp(san_str2, san_str1, strlen(san_str2))==0; //highlight
}
//Returns true if the first chess game starts with the exact same set of moves as the second chess game.
PG_FUNCTION_INFO_V1(hasOpening);
Datum
hasOpening(PG_FUNCTION_ARGS) {

  text *record1 = PG_GETARG_TEXT_PP(0);

  text *record2 = PG_GETARG_TEXT_PP(1);

  bool result = hasOpening_internal(record1,record2); //highlight

  PG_RETURN_BOOL(result);
}

//modify on 12.10
//Returns true if the chessgame contains the given board state in its first N half-moves.

bool hasBoard_internal(text *san_text, SCL_Board *targetBoard, int halfMoves) {

    char *result = NULL;

    char *fenstr = (char*)malloc(70 * sizeof(char));//allocation of size 70 char(FEN notation does not exceed 70 character)

    strcpy(fenstr,"");

    // Iterate through the first N half-moves
    for (int i = 1; i <= halfMoves; ++i) {

        result = getBoard_internal(san_text, i); //highlight

        SCL_boardToFEN(*targetBoard, fenstr);   //highlight

        // Check if the current board matches the target board
        if (strcmp(result, fenstr) == 0) {      //highlight
            return true;  // Found a match
        }
    }

    return false;  // Board not found in the first N half-moves
}

//Returns true if the chessgame contains the given board state in its first N half-moves.
PG_FUNCTION_INFO_V1(hasBoard);
Datum
hasBoard(PG_FUNCTION_ARGS) {

  text *chessgame  = PG_GETARG_TEXT_PP(0);

  char *targetBoard = PG_GETARG_POINTER(1);

  int halfMoves = PG_GETARG_UINT16(2);

  bool result;

  result  = NULL;

  result = hasBoard_internal(chessgame, (SCL_Board *)targetBoard, halfMoves); //highlight

  PG_RETURN_BOOL(result);
}


/*******************************************************************
 * Function to get the first N half moves from PGN
// **************************************************************/
//Returns the chessgame truncated to its first N half-moves.
char* get_first_moves_internal(const char *chessgame, int halfMoves) {

    int N_all = halfMoves + halfMoves/ 2 + halfMoves % 2;
    // Variables to store the result
    char* result = NULL;
    int resultLength = 0;
    // Counter for half moves
    int halfMovesCounter = 0;
    // Pointer to the current position in the PGN
    const char* currentPos = chessgame;

    while (*currentPos != '\0' && halfMovesCounter < N_all) {
        // Skip spaces and move to the next character
        while (*currentPos == ' ') {
            currentPos++;
        }

        // Check for the end of the PGN
        if (*currentPos == '\0') {
            break;
        }

        // Check for the start of a new move
        if (isalnum(*currentPos)) {
            // Increment the halfMoves counter
            halfMovesCounter++;

            // Skip characters until the next space or the end of the PGN
            while (isalnum(*currentPos) || *currentPos == '-') {
                result = realloc(result, resultLength + 1);
                if (result == NULL) {
                    fprintf(stderr, "Memory allocation error\n");
                    exit(EXIT_FAILURE);
                }
                result[resultLength++] = *currentPos;
                currentPos++;
            }
            if(halfMovesCounter%3 == 1){
            // Add a dot after the move number
            result = realloc(result, resultLength + 1);
            if (result == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
            result[resultLength++] = '.';
            // result[resultLength++] = ' ';
            }

            // Add a space between moves if not the last move
            if (halfMovesCounter < N_all) {
                result = realloc(result, resultLength + 1);
                if (result == NULL) {
                    fprintf(stderr, "Memory allocation error\n");
                    exit(EXIT_FAILURE);
                }
                result[resultLength++] = ' ';
            }
        } else {
            // Move to the next character
            currentPos++;
        }
    }

    // Null-terminate the result
    result = realloc(result, resultLength + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    result[resultLength] = '\0';

    return result;
}

PG_FUNCTION_INFO_V1(getFirstMoves);

Datum getFirstMoves(PG_FUNCTION_ARGS) {

  text *chessgame_text = PG_GETARG_TEXT_P(0);

  char *chessgame_str = text_to_cstring(chessgame_text);

  int halfMoves = PG_GETARG_INT32(1);

  if(errorNumCheck(chessgame_text,halfMoves) == true) //highlight
  {
    PG_RETURN_CSTRING(get_first_moves_internal(chessgame_str,halfMoves)); //highlight
  }

  PG_RETURN_CSTRING(NULL);
}

/*******************************************************************
 * B Tree Index Related
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

/*******************************************************************
 * GIN Index Related
**************************************************************/
/*
PG_FUNCTION_INFO_V1(extractValue);
Datum extractValue(PG_FUNCTION_ARGS)
{
    elog(NOTICE, "in extractValue function:");


    text *san_text = PG_GETARG_TEXT_P(0);

    char *san_str = text_to_cstring(san_text);

    SCL_Record record;

    SCL_recordFromPGN(record,san_str);

    int halfMoves = SCL_recordLength(record);

    //test palloc

    // char** board_states;
    // board_states = (char**) palloc(halfMoves * sizeof(char*));
    //SCL_Board *board_states = (char**) palloc(halfMoves * sizeof(char*));

    SCL_Board *board_states = palloc(halfMoves * sizeof(SCL_Board));

    elog(NOTICE, "san_str:%s", san_str);

    elog(NOTICE, "total_half_moves:%d", halfMoves);

    int i;

    char* single_Board = NULL;
    for (i = 0; i <halfMoves; ++i) {

        int j = i+1;

        elog(NOTICE, "In the loop getBoard(i):%d", j);

        single_Board = getBoard_internal(san_text, j);

        elog(NOTICE, "single_Board:%s", single_Board);

        //test palloc

        //board_states[i] = (char*) palloc(sizeof(SCL_Board)); // 请替换为实际的字符串长度

        //board_states[i] = (char*) palloc(sizeof(SCL_Board));

        memcpy(board_states[i], single_Board, sizeof(SCL_Board));

        //pfree(single_Board);
        //memcpy(board_states[i], single_Board, sizeof(SCL_Board));

        elog(NOTICE, "board_states[i]:%s", board_states[i]);
      }

      elog(NOTICE, "After loop");

      for (i = 0; i <halfMoves; ++i) {

        elog(NOTICE, "In 2nd loop,board_states[i]:%s", board_states[i]);

      }

      PG_RETURN_POINTER(board_states);
  }

PG_FUNCTION_INFO_V1(extractQuery);
Datum extractQuery(PG_FUNCTION_ARGS)
{
    elog(NOTICE, "in extractQuery function:");
    // 获取输入的棋盘状态
    text *board_text = PG_GETARG_TEXT_P(0);
    char *board_str = text_to_cstring(board_text);
    //elog(NOTICE, "in extractQuery function:");
    elog(NOTICE, "board_str: %s", board_str);

    PG_RETURN_CSTRING(board_str);
}

PG_FUNCTION_INFO_V1(consistent);
Datum consistent(PG_FUNCTION_ARGS)
{
    //Datum *keys = (Datum *) PG_GETARG_POINTER(0);
    //bool	   *check = (bool *) PG_GETARG_POINTER(0);
    //StrategyNumber strategy = PG_GETARG_UINT16(0);
    //elog(ERROR, "in consistent function: unknown strategy number: %d", strategy);
    elog(NOTICE, "in consistent function:");

    //bool *check = (bool *) PG_GETARG_POINTER(0);
    //StrategyNumber n = PG_GETARG_UINT16(1);
    Datum query = PG_GETARG_DATUM(2);
    int32 nkeys = PG_GETARG_INT32(3);
    Pointer *extra_data = (Pointer *) PG_GETARG_POINTER(4);
    //bool *recheck = (bool *) PG_GETARG_POINTER(5);
    //Datum *queryKeys = (Datum *) PG_GETARG_POINTER(6);
    //bool *nullFlags = (bool *) PG_GETARG_POINTER(7);

  	//bool		res;
  	int32		i;

    int halfMoves = nkeys;

    char *myArray = (char *) DatumGetPointer(extra_data[0]);

    SCL_Board* queryBoard = (SCL_Board*)DatumGetPointer(query);

    char *fenstr;

    SCL_boardToFEN(queryBoard,fenstr);

    //compare boardstates stored in gin index of targetBoard
    for(i = 0; i< halfMoves;i++)
    {
       char *myValue = &myArray[i];
       if(compare_internal(myValue,fenstr)==0)
       {
         PG_RETURN_BOOL(true);
       }
    }
    PG_RETURN_BOOL(false);
}


bool compare_internal(char *state1, char *state2) {
    // compare 2 boardstates
    return strcmp(state1, state2) == 0;
}

PG_FUNCTION_INFO_V1(chessgame_contains);
Datum chessgame_contains(PG_FUNCTION_ARGS) {
    ArrayType *index_states = PG_GETARG_ARRAYTYPE_P(0);
    text *query_state_text = PG_GETARG_TEXT_P(1);
    char *query_state = text_to_cstring(query_state_text);

    Datum *state_datums;
    bool *state_nulls;
    int state_count;
    deconstruct_array(index_states, TEXTOID, -1, false, 'i', &state_datums, &state_nulls, &state_count);

    for (int i = 0; i < state_count; i++) {
        if (state_nulls[i]) continue;
        char *index_state = TextDatumGetCString(state_datums[i]);
        if (compare_internal(query_state, index_state)) {
            PG_RETURN_BOOL(true);
        }
    }
    PG_RETURN_BOOL(false);
}
*/

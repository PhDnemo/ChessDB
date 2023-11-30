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
#include "datatypes.h"

// PostgreSQL module magic
PG_MODULE_MAGIC;

#define MY_BOARD_START_STATE "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define SAN_OUTPUT_SIZE 1024
#define FEN_OUTPUT_SIZE 1024


typedef uint8_t (*SCL_RandomFunction)(void);

/*******************************************************************
 * Input and Output functions  
 * For SAN   
 * 
 * 问题：
 * 1. PGN_OUTPUT_SIZE 没有宏定义；我看.h中没有对pgn的size定义
 * 2. chessgame *game = palloc(sizeof(chessgame)); 中第二个chessgame是不是要改成scl_game
 * 3. 这里面chessgame都要改成scl_game吧？
**************************************************************/

PG_FUNCTION_INFO_V1(san_to_chessgame); // Input
Datum san_to_chessgame(PG_FUNCTION_ARGS) {
    // Retrieve the input SAN string
    char *san_str = PG_GETARG_CSTRING(0);

    // Create a new instance of chessgame structure
    SCL_Game *game = palloc(sizeof(chessgame));

    // Initialize the chessgame structure
    memset(game, 0, sizeof(chessgame));

    // Convert the SAN string to PGN format and store it in game->record
    // Note: Ensure that the conversion from SAN to PGN is feasible
    // If SAN and PGN formats are incompatible, additional conversion logic might be required here
    SCL_recordFromPGN(game->record, san_str);

    // Return the result as your custom type chessgame
    PG_RETURN_POINTER(game);
}

PG_FUNCTION_INFO_V1(chessgame_to_san);
Datum chessgame_to_san(PG_FUNCTION_ARGS) {
    // Retrieve the input chessgame object
    SCL_Game *game = (chessgame *) PG_GETARG_POINTER(0);

    // Create a buffer to store the output PGN string
    char *pgnOutput = (char *) palloc(SAN_OUTPUT_SIZE); // Define PGN_OUTPUT_SIZE to a suitable size
    char *outputPointer = pgnOutput; // Pointer to the current position of the PGN string

    // Define a helper function to append characters to the PGN string
    void appendCharToPGNString(char c, char **str) {
        *(*str) = c; // Set the current character
        (*str)++;    // Move the pointer to the next position
    }

    // Generate PGN string using the SCL_printPGN function
    SCL_printPGN(game->record, appendCharToPGNString, game->currentBoard.board);

    // Ensure the PGN string is null-terminated
    *outputPointer = '\0';

    // Convert the generated PGN string to PostgreSQL's text data type
    text *output_text = cstring_to_text(pgnOutput);

    // Free the allocated memory
    pfree(pgnOutput);

    PG_RETURN_TEXT_P(output_text);
}

/*******************************************************************
 * 11/30 13:30新增
 * Rec and Send functions  
 * For SAN
 * 
 * 问题：
 * 1. 和前面一样的问题，chessgame是不是都要改成SCL_game    //已改
**************************************************************/

PG_FUNCTION_INFO_V1(receive_chessgame);
Datum receive_chessgame(PG_FUNCTION_ARGS) {
    StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);
    SCL_Game *game;

    game = (chessgame *) palloc(sizeof(chessgame));

    game->record = palloc(sizeof(SCL_Game));  // Assuming SCL_Board is the correct type
    game->currentBoard = *game->record;        // Initialize currentBoard, if necessary

    // Read the binary data from the input buffer
    pq_copymsgbytes(buf, (char *) game->record, sizeof(SCL_Game));

    PG_RETURN_POINTER(game);
}

PG_FUNCTION_INFO_V1(send_chessgame);
Datum send_chessgame(PG_FUNCTION_ARGS) {
    SCL_Game *game = (chessgame *) PG_GETARG_POINTER(0);
    StringInfoData buf;

    // Initialize a StringInfoData structure for the output buffer
    pq_begintypsend(&buf);

    // Write the binary data to the output buffer
    pq_sendbytes(&buf, (const char *) game->record, sizeof(SCL_Game));

    // Finalize the output buffer and return it
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/************************************ 
 * Input Output Functions
 * For FEN -- Chessboard
 * 
 * **************************************************/

PG_FUNCTION_INFO_V1(fen_to_chessboard);
Datum fen_to_chessboard(PG_FUNCTION_ARGS) {
    // Retrieve the input FEN string
    char *fenStr = text_to_cstring(PG_GETARG_TEXT_P(0));

    // Create a new instance of the chessboard structure
    SCL_Board *board = palloc(sizeof(chessboard));

    // Load the board state from the FEN string using SCL_boardFromFEN function
    if (!SCL_boardFromFEN(board->board, fenStr)) {
        // If loading fails, release resources and return NULL
        pfree(board);
        PG_RETURN_NULL();
    }

    // Return the newly created chessboard object
    PG_RETURN_POINTER(board);
}

PG_FUNCTION_INFO_V1(chessboard_to_fen);
Datum chessboard_to_fen(PG_FUNCTION_ARGS) {
    // Retrieve the input chessboard object
    SCL_Board *board = (chessboard *) PG_GETARG_POINTER(0);

    // Allocate sufficient memory for the FEN string
    char *fenOutput = (char *) palloc(SCL_FEN_MAX_LENGTH);

    // Convert the board state to a FEN string using the SCL_boardToFEN function
    if (!SCL_boardToFEN(board->board, fenOutput)) {
        // If conversion fails, release resources and return NULL
        pfree(fenOutput);
        PG_RETURN_NULL();
    }

    // Convert the FEN string to PostgreSQL's text data type
    text *output_text = cstring_to_text(fenOutput);

    // Free the allocated memory
    pfree(fenOutput);

    PG_RETURN_TEXT_P(output_text);
}

/*******************************************************************
 * 11/30 13:30新增
 * Rec and Send functions  
 * For fen
 * 
 * 问题：
 * 1. 和前面一样的问题，chessboard是不是都要改成SCL_board     //已改
**************************************************************/

PG_FUNCTION_INFO_V1(receive_chessboard);
Datum receive_chessboard(PG_FUNCTION_ARGS) {
    StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);
    SCL_Board *board;

    board = (chessboard *) palloc(sizeof(chessboard));

    board->board = palloc(sizeof(SCL_Board));  // Assuming SCL_Board is the correct type

    // Read the binary data from the input buffer
    pq_copymsgbytes(buf, (char *) board->board, sizeof(SCL_Board));

    PG_RETURN_POINTER(board);
}

PG_FUNCTION_INFO_V1(send_chessboard);
Datum send_chessboard(PG_FUNCTION_ARGS) {
    SCL_Board *board = (chessboard *) PG_GETARG_POINTER(0);
    StringInfoData buf;

    // Initialize a StringInfoData structure for the output buffer
    pq_begintypsend(&buf);

    // Write the binary data to the output buffer
    pq_sendbytes(&buf, (const char *) board->board, sizeof(SCL_Board));

    // Finalize the output buffer and return it
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/************************************ **************************************************/


uint8_t myRandomFunction() {
    // Your custom randomization logic here
    return rand() % 256;  // Adjust according to your needs
}

typedef struct {
    uint8_t squareFrom;
    uint8_t squareTo;
    char resultProm;
} SCL_MoveResult;


void putCharacter(char c)
{
  putchar(c);
}

char str[4096];

void putCharStr(char c)
{
  char *s = str;

  while (*s != 0)
    s++;

  *s = c;
  *(s + 1) = 0;
}

//Function to return the board state at a given half-move
SCL_Board* getBoard(SCL_Game *chessgame, int halfMoveCount) {
    // 创建一个动态分配的 SCL_Board 结构体
    SCL_Board* result = (SCL_Board*)malloc(sizeof(SCL_Board));

    // 检查内存分配是否成功
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // 复制游戏状态到临时变量
    SCL_Game tempGame;
    memcpy(&tempGame, chessgame, sizeof(SCL_Game));

    // 撤销走子直到达到指定的半步数
    while (tempGame.ply > halfMoveCount) {
        SCL_gameUndoMove(&tempGame);
    }

    // 将结果复制到动态分配的内存中
    memcpy(result, &tempGame.board, sizeof(SCL_Board));

    // 返回动态分配的内存地址
    return result;
}

// Function to get the chessgame truncated to its first N half-moves
SCL_Game getFirstMoves(const SCL_Game* chessgame, int moveCount) {
    SCL_Game tempGame;

    memcpy(&tempGame, chessgame, sizeof(SCL_Game));

    // 撤销走子直到达到指定的半步数
    while (tempGame.ply > moveCount) {
        SCL_gameUndoMove(&tempGame);
    }
    return tempGame;
}

// 判断第一个游戏的开局是否包含在第二个游戏的开局中

bool hasOpening(SCL_Game* game1, SCL_Game* game2) {
    // 创建临时变量以保存两个游戏的状态
    SCL_Game tempGame2;

    // 复制游戏状态到临时变量
    //memcpy(&tempGame1, game1, sizeof(SCL_Game));
    memcpy(&tempGame2, game2, sizeof(SCL_Game));

    // 获取两个游戏的半步数
    uint16_t halfMoves1 = game1->ply;
    uint16_t halfMoves2 = game2->ply;


    // 如果第一个Chess开局的长度大于第二个开局，不可能包含在其中
    if (halfMoves1 > halfMoves2) {
        return false;
    }
    // Undo Chess2,直到halfMoves2与halfMoves1相同
    while (tempGame2.ply > halfMoves1) {
        SCL_gameUndoMove(&tempGame2);
    }

    // 比较两个Game的内容是否完全一致
    return memcpy(&tempGame2, &game1, sizeof(SCL_Game));
}

// Function to check if the chessgame contains the given board state in its first N half-moves
bool hasBoard(SCL_Game* chessgame, const SCL_Board targetBoard, int moveCount) {
    SCL_Board tempBoard;
    SCL_Board* result;

    // Iterate through the first N half-moves
    for (int i = 0; i < moveCount; ++i) {
        // Extract move information from the previous moves
        result = getBoard(chessgame, i);
        memcpy(tempBoard, result, sizeof(SCL_Board));
        // Check if the current board matches the target board
        if (strcmp(tempBoard, targetBoard) == 0) {
            return true;  // Found a match
        }
    }

    return false;  // Board not found in the first N half-moves
}

void printRecord(const SCL_Record record) {
    for (int i = 0; i < SCL_RECORD_MAX_SIZE; ++i) {
        printf("%02x ", record[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
}

int main()
{
    /*Test about Data Types and Transformation*/
    // Create and initialize a chessgame structure
    chessgame game;
    memset(&game, 0, sizeof(game));  // Zero out the structure

    // Create and initialize a chessboard structure
    chessboard board;
    memset(&board, 0, sizeof(board));  // Zero out the structure

    // Test conversion of chessgame to SAN string
    char sanOutput[SAN_OUTPUT_SIZE];
    chessgameToSAN(&game, sanOutput);
    printf("SAN Output: %s\n", sanOutput);

    // Test conversion of chessboard to FEN string
    char fenOutput[FEN_OUTPUT_SIZE];
    chessboardToFEN(&board, fenOutput);
    printf("FEN Output: %s\n", fenOutput);



//     SCL_Game game;
//     SCL_Board initial_board, result_board;
//     SCL_SquareSet emptySquareSet = SCL_SQUARE_SET_EMPTY;
//     SCL_Board board = SCL_BOARD_START_STATE;

//     // initialization of the chess game, including moves
//     SCL_gameInit(&game,board);

//     memcpy(initial_board, game.board, sizeof(SCL_Board));
//     printf("Print board from initial game+++++++++++++++: \n");
//     SCL_printBoard(initial_board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8, 4, 1, 0);

//     // ********************TestCase of getBoard function ********************
//     // Get the initial board state
//     SCL_Board* result = getBoard(&game, 0);
//     memcpy(initial_board, result, sizeof(SCL_Board));

//     printf("Print initial_board from Game->getBoard0: \n");
//     SCL_printBoard(initial_board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8, 4, 1, 0);

//    //1. Nf3 Nf6 2. c4 g6 3. Nc3 Bg7 4. d4 O-O 5. Bf4 d5
//    // test 3 halfMoves
//     SCL_gameMakeMove(&game, 12, 21, '\0');// 1-1. Nf3
//     //printf("Print record after 1st Move: \n");
//     //printRecord(game.record);
//     printf("Printboard after SCL_gameMakeMoves 1st,before getBoard: \n");
//     SCL_printBoard(game.board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);
//     printf("Printboard after SCL_gameMakeMoves,after getBoard: \n");
//     result = getBoard(&game, 1);
//     memcpy(initial_board, result, sizeof(SCL_Board));
//     SCL_printBoard(initial_board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);
//     free(result);

//     SCL_gameMakeMove(&game, 62, 45, '\0');// 1-2. Nf6
//   //  printf("Print record after 2nd Move: \n");
//   //  printRecord(game.record);
//     printf("Printboard after SCL_gameMakeMoves 2nd,before getBoard: \n");
//     SCL_printBoard(game.board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);
//     printf("Printboard after SCL_gameMakeMoves,after getBoard: \n");
//     result = getBoard(&game, 2);
//     memcpy(initial_board, result, sizeof(SCL_Board));
//     SCL_printBoard(initial_board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);
//     free(result);

//     result = getBoard(&game, 1);
//     memcpy(initial_board, result, sizeof(SCL_Board));
//     printf("Printboard after SCL_gameMakeMoves 2,after getBoard1: \n");
//     SCL_printBoard(initial_board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);
//     free(result);

//     SCL_gameMakeMove(&game, 2, 18, '\0');  // 2-1. c4
//     printf("Printboard after SCL_gameMakeMoves 3rd,before getBoard: \n");
//     SCL_printBoard(game.board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);
//     printf("Printboard after SCL_gameMakeMoves,after getBoard: \n");
//     result = getBoard(&game, 3);
//     memcpy(initial_board, result, sizeof(SCL_Board));
//     SCL_printBoard(initial_board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);
//     free(result);

//     result = getBoard(&game, 2);
//     memcpy(initial_board, result, sizeof(SCL_Board));
//     printf("Printboard after SCL_gameMakeMoves 3,after getBoard2\n: ");
//     SCL_printBoard(initial_board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);
//     free(result);

// /*
//     SCL_gameMakeMove(&game, 29, 47, '\0');  // 2-2. g6
//     SCL_gameMakeMove(&game, 22, 37, '\0');  // 5. Nc3
//     SCL_gameMakeMove(&game, 9, 42, '\0');  // 6. Bg7
//     SCL_gameMakeMove(&game, 3, 26, '\0');  // 7. d4
//     SCL_gameMakeMove(&game, 57, 57, '\0');  // 8. O-O
//     SCL_gameMakeMove(&game, 26, 46, '\0');  // 9. Bf4
//     SCL_gameMakeMove(&game, 5, 37, '\0');  // 10. d5
// */

//     // ********************TestCase of getFirstMoves function********************

//      SCL_Game truncatedGame = getFirstMoves(&game, 2);

//      printf("Printboard after getFirstMoves: \n");

//      SCL_printBoard(truncatedGame.board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);

//      result = getBoard(&truncatedGame, 2);
//      memcpy(initial_board, result, sizeof(SCL_Board));
//      printf("Printboard after getFirstMoves: 2,after getBoard2\n: ");
//      SCL_printBoard(initial_board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);
//      free(result);

//    // ********************TestCase of hasOpenings function********************
//     if (hasOpening(&truncatedGame, &game))
//     {
//         printf("The opening moves match!\n");
//     }
//     else
//     {
//         printf("The opening moves do not match!\n");
//     }


//     // ********************TestCase of hasBoard function********************

//     SCL_Board* result1 = getBoard(&game, 2);

//     SCL_Board target_board;

//     memcpy(target_board, result1, sizeof(SCL_Board));

//     free(result1);

//     printf("Print target_board**************: \n");

//     SCL_printBoard(target_board, putCharacter, emptySquareSet, 255, SCL_PRINT_FORMAT_UTF8,4,1,0);

//     if (hasBoard(&game, target_board, 3))
//     {
//         printf("The target board state is found in the first 2half-moves!\n");
//     }
//     else
//     {
//         printf("The target board state is not found in the first 2 half-moves!\n");
//     }

    return 0;
}

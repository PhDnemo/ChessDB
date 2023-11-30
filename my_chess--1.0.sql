
\echo Use "CREATE EXTENSION my_chess" to load this file. \quit


-- Create input and output function for SEN and FEN

CREATE OR REPLACE FUNCTION san_to_chessgame(text)
    RETURNS scl_game
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessgame_to_san(scl_game)
    RETURNS text
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


CREATE OR REPLACE FUNCTION fen_to_chessboard(text)
    RETURNS scl_board
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessboard_to_fen(scl_board)
    RETURNS text
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- Create type
CREATE TYPE scl_game (
  internallength = 1024,
  input          = san_to_chessgame,
  output         = chessgame_to_san
);

CREATE TYPE scl_board (
  internallength = 1024,
  input          = fen_to_chessboard,
  output         = chessboard_to_fen
);

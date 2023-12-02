-- Use "CREATE EXTENSION my_chess" to load this file.

-- Create input and output functions for SEN and FEN
-- Uncomment and adjust the function declarations as needed
-- CREATE OR REPLACE FUNCTION san_to_chessgame(text)
--     RETURNS scl_game
--     AS 'MODULE_PATHNAME'
--     LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- CREATE OR REPLACE FUNCTION chessgame_to_san(scl_game)
--     RETURNS text
--     AS 'MODULE_PATHNAME'
--     LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE TYPE chessboard;
CREATE TYPE chessgame;
CREATE OR REPLACE FUNCTION SCL_board_in(cstring)
    RETURNS chessboard
    AS 'MODULE_PATHNAME','SCL_board_in'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION SCL_board_out(chessboard)
    RETURNS cstring
    AS 'MODULE_PATHNAME','SCL_board_out'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION san_in(cstring)
    RETURNS chessgame
    AS 'MODULE_PATHNAME','san_in'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION san_out(chessgame)
    RETURNS cstring
    AS 'MODULE_PATHNAME','san_out'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- Create chessboard datatype
CREATE TYPE chessboard (
  internallength = 1024,
  input          = SCL_board_in,
  output         = SCL_board_out
);
-- Create chessgame datatype
CREATE TYPE chessgame (
  internallength = 1024,
  input          = san_in,
  output         = san_out
);

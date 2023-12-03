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

CREATE OR REPLACE FUNCTION getBoard(chessgame,int)
  RETURNS cstring
  AS 'MODULE_PATHNAME','getBoard'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION hasOpening(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'hasOpening'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
  
CREATE FUNCTION hasBoard(chessgame, chessboard,int)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'hasBoard'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION getFirstMoves(chessgame,int)
  RETURNS cstring
  AS 'MODULE_PATHNAME','getFirstMoves'
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


/******************************************************************************
 * Operators for B-Tree
 ******************************************************************************/

CREATE FUNCTION chessgame_cmp(chessgame, chessgame)
  RETURNS integer
  AS 'MODULE_PATHNAME', 'chessgame_cmp'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_eq(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_eq'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_ne(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_ne'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_lt(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_lt'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_le(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_le'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_gt(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_lt'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_ge(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_le'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- Equal to
CREATE OPERATOR = (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = chessgame_eq,
  COMMUTATOR = =, 
  NEGATOR = <>
);
-- Not equal
CREATE OPERATOR <> (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = chessgame_ne,
  COMMUTATOR = <>, 
  NEGATOR = =
);
-- Less than
CREATE OPERATOR < (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = chessgame_lt,
  COMMUTATOR = >,
  NEGATOR = >=
);
-- Less than or equal
CREATE OPERATOR <= (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = chessgame_le,
  COMMUTATOR = >=,
  NEGATOR = >
);
-- Greater than
CREATE OPERATOR > (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = chessgame_gt,
  COMMUTATOR = <,
  NEGATOR = <=
);
--Greater than or equal
CREATE OPERATOR >= (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = chessgame_ge,
  COMMUTATOR = <=,
  NEGATOR = <
);

CREATE OPERATOR CLASS chessgame_btree
DEFAULT FOR TYPE chessgame USING btree AS
  OPERATOR 1 < (chessgame, chessgame),
  OPERATOR 2 <= (chessgame, chessgame),
  OPERATOR 3 = (chessgame, chessgame),
  OPERATOR 4 >= (chessgame, chessgame),
  OPERATOR 5 > (chessgame, chessgame),
  FUNCTION 1 chessgame_cmp(chessgame, chessgame);

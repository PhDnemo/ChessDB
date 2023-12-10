-- Use "CREATE EXTENSION my_chess" to load this file.

CREATE TYPE chessboard;
CREATE TYPE chessgame;

CREATE OR REPLACE FUNCTION chessboard_in(cstring)
  RETURNS chessboard
  AS 'MODULE_PATHNAME','chessboard_in'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessboard_out(chessboard)
  RETURNS cstring
  AS 'MODULE_PATHNAME','chessboard_out'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessgame_in(cstring)
  RETURNS chessgame
  AS 'MODULE_PATHNAME','chessgame_in'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessgame_out(chessgame)
  RETURNS cstring
  AS 'MODULE_PATHNAME','chessgame_out'
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
  input          = chessboard_in,
  output         = chessboard_out
);
-- Create chessgame datatype
CREATE TYPE chessgame (
  internallength = 1024,
  input          = chessgame_in,
  output         = chessgame_out
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


/*******************************************************************
 * GIN Index Related
**************************************************************/

-- CREATE FUNCTION chessgame_contains(chessgame, chessgame)
--   RETURNS boolean
--   AS 'MODULE_PATHNAME', 'chessgame_contains'
--   LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
--
-- -- 定义 @> 操作符
-- CREATE OPERATOR @> (
--     LEFTARG = chessgame,
--     RIGHTARG = chessgame,
--     PROCEDURE = chessgame_contains
--     -- NEGATOR = <@
-- );
-- -- CREATE FUNCTION extractValue(chessgame, int)
-- CREATE FUNCTION extractValue(chessgame)
--   RETURNS SETOF string
--   -- RETURNS bool
--   AS 'MODULE_PATHNAME', 'extractValue'
--   LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
--
-- CREATE FUNCTION extractQuery(internal,chessgame,chessboard,int)
--   RETURNS cstring
--   AS 'MODULE_PATHNAME', 'extractQuery'
--   LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
--
-- CREATE FUNCTION consistent(int, chessboard, int, chessgame)
--   RETURNS bool
--   AS 'MODULE_PATHNAME', 'consistent'
--   LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
--
-- CREATE FUNCTION compare(chessboard,chessboard)
--   RETURNS boolean
--   AS 'MODULE_PATHNAME', 'compare'
--   LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
--
-- CREATE OPERATOR CLASS chessgame_gin_ops
-- DEFAULT FOR TYPE chessgame USING gin AS
--     OPERATOR 1 @>,
--     FUNCTION 1 compare(chessboard,chessboard),
--     FUNCTION 2 extractValue(chessgame),
--     FUNCTION 3 extractQuery(internal, chessgame,chessboard,int),
--     FUNCTION 4 consistent(int, chessboard, int, chessgame),
--     STORAGE text; -- GIN storage format

DROP EXTENSION IF EXISTS my_chess CASCADE;

CREATE EXTENSION my_chess;

-- find extension
-- SELECT extname
-- FROM pg_extension;

DROP TABLE IF EXISTS t;
-- create table
CREATE TABLE t (
    id SERIAL PRIMARY KEY,
    Event TEXT,
    Site TEXT,
    Date TEXT,
    Round INT,
    White TEXT,
    Black TEXT,
    Result TEXT,
    Moves chessgame
);

-- import data
-- modify the dir </Users/zhangmin/Documents/ULB/417/gin_test/Data Source>to your file dir
\COPY t(Event, Site, Date, Round, White, Black, Result, Moves) FROM '/Users/zhangmin/Documents/ULB/417/gin_test/Data Source/Adams4.csv' DELIMITER ',' CSV HEADER;

--\COPY t(Event, Site, Date, Round, White, Black, Result, Moves) FROM '/home/jintao/Downloads/test/Adams4.csv' DELIMITER ',' CSV HEADER;
-- -- view data type
-- \dT+
--
-- -- view table
-- \d t


-- 1.getBoard test case

/* Run these Linux command in the folder of fen files to aggregate them together:
-- sed -i -e '$a\' 1.fen
-- cat 1.fen 2.fen 3.fen 4.fen 5.fen 6.fen > total.fen
*/
/* https://www.apronus.com/chess/pgnviewer/   Implementing one game and record SAN and FEN to verify our function*/
DROP TABLE IF EXISTS fen_data;
-- create fen table
CREATE TABLE fen_data (
    id SERIAL PRIMARY KEY,
    fen TEXT
);

-- import fen data
\COPY fen_data(fen) FROM '/Users/zhangmin/Documents/ULB/417/gin_test/Data Source/total.fen' WITH (FORMAT text);
--\COPY fen_data(fen) FROM '/home/jintao/Pictures/Screenshots/Test_Case/getBoard/verify/total.fen' WITH (FORMAT text);

\pset pager off

-- insert SAN data for testing
INSERT INTO t(Moves) VALUES('1. e3 d5 2. g4 Bxg4 3. e4 Bxd1 *');

-- Query if the generated_fen euqal the true fen in our chess website and return two string and T/F
SELECT
    getBoard(t.Moves, 6)::text AS generated_fen,
    f.fen AS fen_data,
    (CASE
        WHEN getBoard(t.Moves, 6)::text = f.fen THEN true
        ELSE false
    END) AS is_match
FROM
    t,
    fen_data f
WHERE
    t.Moves = '1. e3 d5 2. g4 Bxg4 3. e4 Bxd1 *' AND
    f.id = 6;


-- View if two games have the same board after 3 half-moves
SELECT CAST(getBoard(t.Moves, 3) AS text) AS board, COUNT(*) AS count
FROM t
WHERE id IN (1, 2)
GROUP BY CAST(getBoard(t.Moves, 3) AS text);

-- Query the most common board  after 3 half-moves in games where Adam is playing as White, and the number of times this position occurs
SELECT CAST(getBoard(t.Moves, 3) AS TEXT) AS chess_board, COUNT(*) AS count
FROM t
WHERE White = 'Adams, Michael'
GROUP BY CAST(getBoard(t.Moves, 3) AS TEXT)
ORDER BY count DESC
LIMIT 1;

-- Query the most frequent board in games where Adam was playing as Black and won the game, and the number of times this position occurs
SELECT CAST(getBoard(t.Moves, 3) AS TEXT) AS chess_board, COUNT(*) AS count
FROM t
WHERE Black = 'Adams, Michael' AND Result='0-1'
GROUP BY CAST(getBoard(t.Moves, 3) AS TEXT)
ORDER BY count DESC
LIMIT 1;

-- 2.getFirstMoves test case

-- Query the first five half-moves of the second game
select getFirstMoves(t.Moves,5) from t where id=2;

-- Query the most frequent first 3 half-moves in games when Adam was playing as White
SELECT CAST(getFirstMoves(t.Moves, 3) AS TEXT) AS first_moves, COUNT(*) AS move_count
FROM t
WHERE White = 'Adams, Michael'
GROUP BY first_moves
ORDER BY move_count DESC
LIMIT 1;

-- Query the most frequent first 3 half-moves in games when Adam was playing as Black and won the game
SELECT CAST(getFirstMoves(t.Moves, 3) AS TEXT) AS first_moves, COUNT(*) AS move_count
FROM t
WHERE Black = 'Adams, Michael' AND Result='0-1'
GROUP BY first_moves
ORDER BY move_count DESC
LIMIT 1;

-- 3.hasopening test case
-- Query the number of games starting with '1. e4 e6'
SELECT count(*)
FROM t
WHERE hasopening(Moves, '1. e4 e6');

-- Query the id of games starting with '1. e4 e6' and Black won the game
SELECT id
FROM t
WHERE hasopening(Moves,'1. e4 c6 2. d4 d5') AND Result='0-1';


-- 4.hasboard test case
/* https://www.chess-poster.com/english/lt_pgn_to_fen/lt_pgn_fen.htm   Using this website to tranform SAN inro FEN*/

DROP TABLE IF EXISTS fen_data;

-- create fen table
CREATE TABLE fen_data (
    id SERIAL PRIMARY KEY,
    fen TEXT
);

-- import fen data
\COPY fen_data(fen) FROM '/Users/zhangmin/Documents/ULB/417/gin_test/Data Source/total.fen' WITH (FORMAT text);
--\COPY fen_data(fen) FROM '/home/jintao/Pictures/Screenshots/Test_Case/getBoard/verify/total.fen' WITH (FORMAT text);

\pset pager off

-- View the fen column of fen_data
select fen from fen_data;

 --COPY fen to query with the
SELECT hasboard(Moves,
'rnbqkbnr/ppp1pppp/8/3p4/8/4P3/PPPP1PPP/RNBQKBNR w KQkq d6 0 2', 10) from t where id=3486;
--Query the number of chessgames beginning with 'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1' equal to 1e4
SELECT count(*)
FROM t
WHERE hasboard(Moves,
'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1', 10);

--Query the number of chessgames beginning with 'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1' equal to 1nf3
SELECT count(*)
FROM t
WHERE hasboard(Moves,
'rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R b KQkq - 1 1', 10);

--Query the number of chessgames beginning with 'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1' equal to 1nf3 nf6
SELECT count(*)
FROM t
WHERE hasboard(Moves,
'rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 2 2', 10);

-- insert SAN data
INSERT INTO t(Moves) VALUES('1. e3 d5 2. g4 Bxg4 3. e4 Bxd1 *');

/* Run these Linux command in the folder of fen files to aggregate them together:
-- sed -i -e '$a\' 1.fen
-- cat 1.fen 2.fen 3.fen 4.fen 5.fen 6.fen > total.fen
*/

-- create a bigger table named t2 just for B tree test
DROP TABLE IF EXISTS t2;

-- create table
CREATE TABLE t2 (
    id SERIAL PRIMARY KEY,
    Event TEXT,
    Site TEXT,
    Date TEXT,
    Round INT,
    White TEXT,
    Black TEXT,
    Result TEXT,
    Moves chessgame
);

\COPY t(Event, Site, Date, Round, White, Black, Result, Moves) FROM '/Users/zhangmin/Documents/ULB/417/gin_test/Data Source/TrompowskyOther4.csv' DELIMITER ',' CSV HEADER;

--\COPY t(Event, Site, Date, Round, White, Black, Result, Moves) FROM '/home/jintao/Downloads/test/TrompowskyOther4.csv' DELIMITER ',' CSV HEADER;

-- 5.b-tree test case
/* I tried two dataset:the first one(t) had 3000 rows,the second one(t2) had 17000rows.And execution time of query with B Tree performed better than without B Tree in the second dataset.But the first one did not have better performance with B tree.So I thought the volumn of data would affect the performance of B Tree*/

--test for t
DROP INDEX IF EXISTS my_chessgame_index;

CREATE INDEX my_chessgame_index ON t USING btree (Moves chessgame_btree);

-- with index
SET enable_seqscan = OFF;
EXPLAIN (ANALYZE, BUFFERS, TIMING ON) SELECT hasOpening('1. e3 Nh6', Moves) FROM t;

-- without index
SET enable_seqscan = ON;
EXPLAIN (ANALYZE, BUFFERS, TIMING ON) SELECT hasOpening('1. e3 Nh6', Moves) FROM t;


--test for t2
DROP INDEX my_chessgame_index;

CREATE INDEX my_chessgame_index ON t2 USING btree (Moves chessgame_btree);

-- with index
SET enable_seqscan = OFF;
EXPLAIN (ANALYZE, BUFFERS, TIMING ON) SELECT hasOpening('1. e3 Nh6', Moves) FROM t2;

-- without index
SET enable_seqscan = ON;
EXPLAIN (ANALYZE, BUFFERS, TIMING ON) SELECT hasOpening('1. e3 Nh6', Moves) FROM t2;

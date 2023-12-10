# ChessDB
This is the DBSA project.


## Contributors

- **Min Zhang** - *1* - [Min](https://github.com/PhDnemo)
- **Yutao Cheng** - *2* - [Yutao](https://github.com/A-hungry-wolf)
- **Ziyong Zhang** - *3* - [Ziyong](https://github.com/Ziyong-Zhang)
- **Jintao Ma** - *4* - [Jintao](https://github.com/woshimajintao)

## System Requirements

This project requires the following system environment:
- **Operating System**: 
- **Postgres Version**: 

## Environment Setup

- **Install **: 



## Usage
To run the extension, follow these steps:

(1) Run the below in the terminal:

```bash
PATH=/usr/local/pgsql/bin:$PATH
cd “path_to_mychess”/complex/
make clean
make
sudo make install

psql -p <2345> chess_test

```
(2) 
``` sql
DROP EXTENSION IF EXISTS my_chess CASCADE;

CREATE EXTENSION my_chess;

DROP TABLE IF EXISTS t ;
CREATE TABLE t(id int, board chessboard);

insert into t VALUES (2, 'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1');

SELECT * FROM t;

DROP TABLE IF EXISTS t1 ;
CREATE TABLE t1(id int, game chessgame);

insert into t1 VALUES (1, '1. Nf3 Nf6 2. c4 g6 3. Nc3 Bg7');

select * from t1;

select getBoard(t1.game,3) as chess_board from t1;

DROP TABLE IF EXISTS t2 ;
CREATE TABLE t2(id int, game1 chessgame,game2 chessgame);

insert into t2 VALUES (1, '1. e3 Nh6 2. Nc3 c5 3. Bb5 e6 4. g4 f5 5. g5 Qxg5','1. e3 Nh6');

select hasOpening(game1,game2) from t2;    

drop table if exists t3;

create table t3(id int, game chessgame, board chessboard);

insert into t3 values(1,'1.f4 Nc6 2.f5 h5 3. b3 Nd4 4. b4 c6 5. g4 hxg4 6. d3 Nxf5 7. Nc3 g3 8. hxg3 Nxg3','r1bqkbnr/pp1pppp1/2p5/8/1P6/2NP2n1/P1P1P3/R1BQKBNR w KQkq - 0 9');

select getFirstMoves(t3.game,5) from t3;




DROP TABLE IF EXISTS t4 ;

CREATE TABLE t4(id int, game chessgame, board chessboard);  

insert into t4 values(1,'1. d3 d6 2. Nf3 Nf6 3. c3 d5 4. e4 e5','rnbqkb1r/ppp1pppp/3p1n2/8/8/3P1N2/PPP1PPPP/RNBQKB1R w KQkq - 2 3');

select hasBoard(t4.game,t4.board,6) from t4;


-- has opening_ b-tree
DROP TABLE IF EXISTS t5 ;

CREATE TABLE t5(id int, game chessgame);

insert into t5 VALUES (1, '1. e3 Nh6 2. Nc3 c5 3. Bb5 e6 4. g4 f5 5. g5 Qxg5');

create index on t5(game);

set enable_seqscan = off;

explain(costs on) select hasOpening('1. e3 Nh6',game) from t5;



## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)

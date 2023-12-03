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

CREATE TABLE t(id int, board chessboard);

insert into t VALUES (2, 'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1');

SELECT * FROM t;

CREATE TABLE t1(id int, game chessgame);

insert into t1 VALUES (1, '1. Nf3 Nf6 2. c4 g6 3. Nc3 Bg7');

select * from t1;

select getBoard(t1.game,3) as chess_board from t1;



## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)

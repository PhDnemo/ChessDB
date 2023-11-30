# Makefile
MODULE_big = my_chess
OBJS = my_chess.o

EXTENSION = my_chess
DATA = my_chess--1.0.sql my_chess.control

PG_CONFIG ?= pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

CC=g++
FLAGS=-g --std=c++20 -o 

SRC=$(wildcard src/*.cpp)
DEPS=$(wildcard inc/*.h)

all: main

main: $(SRC) main.cpp
	$(CC) $^ $(FLAGS) $@


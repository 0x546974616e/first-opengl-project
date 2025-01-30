
.PHONY: all run crun

all:
	clang++-19 -c ./sources/main.cpp -o ./build/main.o -I ./headers
	clang-19 -c ./sources/glad.c -o ./build/glad.o -I ./headers
	clang++-19 ./build/main.o ./build/glad.o -o ./build/main -lglfw

run:
	@./build/main

crun: all run

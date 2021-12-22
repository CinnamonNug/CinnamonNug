MAIN = main.c
CC = gcc
FLAGS = -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
default:
	$(CC) $(MAIN) -o game.exe $(FLAGS)
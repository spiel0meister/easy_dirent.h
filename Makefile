CC=gcc -std=c11
CFLAGS=-Wall -Wextra -ggdb

all: easy-dirent

easy-dirent: main.c
	$(CC) $(CFLAGS) -o $@ $<

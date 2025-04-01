CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -fopenmp
LDFLAGS = -lm

main: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
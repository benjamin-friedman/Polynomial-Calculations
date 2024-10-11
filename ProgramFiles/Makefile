# Author:       Benjamin G. Friedman
# Date:         01/02/2022
# File:         Makefile
# Description:  Makefile for the polynomial calculations program.


CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic #-Og -g -fsanitize=undefined
LDLIBS = -lm
EXE1 = PolynomialCalculations
OBJ1 = Main.o Poly.o Menu.o
EXES = $(EXE1)


all: $(EXES)
.PHONY: all clean


$(EXE1): $(OBJ1)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm $(EXES) $(wildcard *.o)

# Define required macros here
SHELL = /bin/sh

MAIN_A = 03_a
MAIN_B = 03_b
CFLAGS = -Wall -g -std=c11
CC = gcc

%: %.o
	${CC} ${CFLAGS} -o $@ 

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ 

clean:
	rm -f *.o ${MAIN_A} ${MAIN_B}

test_a: ${MAIN_A}
	valgrind ./${MAIN_A} < input.txt

test_b: ${MAIN_B}
	valgrind ./${MAIN_B} < input.txt

all: test_a test_b
	make clean
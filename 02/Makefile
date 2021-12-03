# Define required macros here
SHELL = /bin/sh

MAIN_A = 02_a
MAIN_B = 02_b
CFLAGS = -Wall -g -std=c11
CC = gcc

%: %.o
	${CC} ${CFLAGS} -o $@ 

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ 

clean:
	rm -f *.o ${MAIN_A} ${MAIN_B}

test_a: ${MAIN_A}
	./${MAIN_A} < input.txt

test_b: ${MAIN_B}
	./${MAIN_B} < input.txt

all: test_a test_b
	make clean
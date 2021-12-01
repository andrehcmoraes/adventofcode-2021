# Define required macros here
SHELL = /bin/sh

MAIN_A = 01_a
MAIN_B = 01_b
CFLAG = -Wall -g -std=c11
CC = gcc

%: %.o
	${CC} ${CFLAGS} -o $@ 

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ 

clean:
	rm -f *.o ${MAIN_A} ${MAIN_B}

test_a: ${MAIN_A}
	./${MAIN_A} < 01.txt

test_b: ${MAIN_B}
	./${MAIN_B} < 01.txt

all: test_a test_b
	make clean
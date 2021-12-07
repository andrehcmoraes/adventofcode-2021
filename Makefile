#!/usr/bin/make -f
# Thanks to https://makefiletutorial.com/

SHELL = /bin/bash
CFLAGS = -Wall -g -std=c11
CC = gcc

# Generate rules for each day

.PHONY: clean
clean:
	rm -f *.o *.bin

# Make and run with BIGNUM
day%_bn:
	@${CC} -DUSE_BIG_ENUM ${CFLAGS} $*/$*_b.c -o $*_b.bin -lssl -lcrypto
	./$*_b.bin < $*/input.txt
	@$(MAKE) clean > /dev/null


%.out:
	@${CC} ${INC_FLAGS} ${CFLAGS} $*/$*_a.c -o $*_a.bin
	@${CC} ${INC_FLAGS} ${CFLAGS} $*/$*_b.c -o $*_b.bin

# Make and run
day%: %.out
	@echo "Day $* Part One"
	./$*_a.bin < $*/input.txt
	@echo ""
	@echo "Day $* Part Two"
	./$*_b.bin < $*/input.txt
	@$(MAKE) clean > /dev/null

# Debug test samples
test%: %.out
	@echo "Debug $* Part One"
	valgrind ./$*_a.bin < $*/test.txt
	@echo ""
	@echo "Debug $* Part Two"
	valgrind ./$*_b.bin < $*/test.txt
	@$(MAKE) clean > /dev/null

# Debug full input
debug%: %.out
	@echo "Debug $* Part One"
	valgrind ./$*_a.bin < $*/input.txt
	@echo ""
	@echo "Debug $* Part Two"
	valgrind ./$*_b.bin < $*/input.txt
	@$(MAKE) clean > /dev/null

# Time execution
time%: %.out
	@echo "Day $* Part One"
	time ./$*_a.bin < $*/input.txt
	@echo ""
	@echo "Day $* Part Two"
	time ./$*_b.bin < $*/input.txt
	@$(MAKE) clean > /dev/null

CC=clang
CFLAGS=-ansi -pedantic -O3 -g3 -Wall -Wextra -Werror -g -fsanitize=address

test: cesu8.h test.c
	$(CC) $(CFLAGS) -o test test.c

clean:
	rm -f test


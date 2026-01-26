CC = gcc
CFLAGS = -Iinclude

bin/lc3: src/main.c src/memory.c src/input.c
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm -f bin/lc3
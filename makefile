CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

.PHONY: all clean

all: quantization

quantization: main.o inputParser.o queryHandler.o trie.o dsu.o
	$(CC) -o quantization $^

main.o: main.c inputParser.h queryHandler.h
	$(CC) $(CFLAGS) -c $<

inputParser.o: inputParser.c inputParser.h
	$(CC) $(CFLAGS) -c $<

queryHandler.o: queryHandler.c queryHandler.h trie.h dsu.h
	$(CC) $(CFLAGS) -c $<

trie.o: trie.c trie.h dsu.h
	$(CC) $(CFLAGS) -c $<

dsu.o: dsu.c dsu.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o quantization

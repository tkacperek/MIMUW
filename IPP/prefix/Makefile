CC = gcc -Wall -std=c99
CCG = gcc -Wall -g -std=c99

dictionary: dictionary.c config.h sptr.h sptr.o parse.h parse.o trie.h trie.o
	$(CC) -o dictionary dictionary.c sptr.o parse.o trie.o

sptr.o: config.h sptr.h sptr.c
	$(CC) -c sptr.c

parse.o: config.h sptr.h sptr.o parse.h parse.c trie.h trie.o
	$(CC) -c parse.c

trie.o: config.h sptr.h sptr.o trie.h trie.c
	$(CC) -c trie.c


debug: dictionary.c config.h sptr.h sptr.dbg.o parse.h parse.dbg.o trie.h trie.dbg.o
	$(CCG) -o dictionary.dbg dictionary.c sptr.dbg.o parse.dbg.o trie.dbg.o

sptr.dbg.o: config.h sptr.h sptr.c
	$(CCG) -c -o sptr.dbg.o sptr.c

parse.dbg.o: config.h sptr.h sptr.dbg.o parse.h parse.c trie.h trie.dbg.o
	$(CCG) -c -o parse.dbg.o parse.c

trie.dbg.o: config.h sptr.h sptr.dbg.o trie.h trie.c
	$(CCG) -c -o trie.dbg.o trie.c

clean:
	-rm -f *.o
	-rm -f -r *.dSYM
	-rm -f dictionary dictionary.dbg

#CC=gcc
#OPTS=-g -Werror -std=gnu++0x
#
#all: main.o cache.o util.o
#	$(CC) $(OPTS) -o cache main.o cache.o util.o -lstdc++ -lm
#
#main.o: main.cpp
#	$(CC) $(OPTS) -c main.cpp -lstdc++
#
#cache.o: cache.cpp cache.h
#	$(CC) $(OPTS) -c cache.cpp -lstdc++
#
#util.o: util.cpp util.h
#	$(CC) $(OPTS) -c util.cpp -lstdc++
#
#clean:
#	rm -f *.o cache;


#For macOS:

CC=g++
OPTS=-g -Werror -std=gnu++0x

all: main.o cache.o util.o
	$(CC) $(OPTS) -o cache main.o cache.o util.o

main.o: main.cpp cache.h
	$(CC) $(OPTS) -c main.cpp

cache.o: cache.h cache.cpp
	$(CC) $(OPTS) -c cache.cpp

util.o: util.cpp util.h
	$(CC) $(OPTS) -c util.cpp

clean:
	rm -f *.o cache;
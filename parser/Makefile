# flags for compiling a .o file
OFLAGS = -Wall -std=c11 -o $@ -c -g -I/usr/include/libxml2
FLAGS = -Wall -std=c11  -pedantic -o $@ -g

PARSER_SRC_FILES = $(wildcard src/GPX*.c)
PARSER_OBJ_FILES = $(patsubst src/GPX%.c,GPX%.o,$(PARSER_SRC_FILES))


library: libgpxparser.so

test: libgpxparser.so liblist.so mainTest.c
	gcc $(FLAGS) -I/usr/include/libxml2 -lxml2 -lm mainTest.c GPXParser.o GPXhelpers.o LinkedListAPI.o



libgpxparser.so: $(PARSER_OBJ_FILES) LinkedListAPI.o
	gcc -shared -o ../libgpxparser.so $(PARSER_OBJ_FILES) LinkedListAPI.o -lxml2 -lm

liblist.so: LinkedListAPI.o
	gcc -shared -o $@ LinkedListAPI.o



LinkedListAPI.o: src/LinkedListAPI.c include/LinkedListAPI.h
	gcc $(OFLAGS) -fpic src/LinkedListAPI.c -Isrc/ -Iinclude/

GPX%.o: src/GPX%.c include/LinkedListAPI.h include/GPX*.h
	gcc $(OFLAGS) -I/usr/include/libxml2 -Iinclude/ -fpic $<


clean: 
	rm *.o ../libgpxparser.so

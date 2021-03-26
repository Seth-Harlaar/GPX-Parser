# flags for compiling a .o file
OFLAGS = -Wall -std=c11 -o $@ -c -g -I/usr/include/libxml2
FLAGS = -Wall -std=c11  -pedantic -o $@ -g

PARSER_SRC_FILES = $(wildcard GPX*.c)
PARSER_OBJ_FILES = $(patsubst GPX%.c,GPX%.o,$(PARSER_SRC_FILES))


library: libgpxparser.so

test: libgpxparser.so liblist.so mainTest.c
	gcc $(FLAGS) -I/usr/include/libxml2 -lxml2 -lm mainTest.c GPXParser.o GPXhelpers.o LinkedListAPI.o



libgpxparser.so: $(PARSER_OBJ_FILES) LinkedListAPI.o
	gcc -shared -o ../libgpxparser.so $(PARSER_OBJ_FILES) LinkedListAPI.o -lxml2 -lm

liblist.so: LinkedListAPI.o
	gcc -shared -o $@ LinkedListAPI.o



LinkedListAPI.o: LinkedListAPI.c LinkedListAPI.h
	gcc $(OFLAGS) -fpic LinkedListAPI.c

GPX%.o: GPX%.c LinkedListAPI.h GPX*.h
	gcc $(OFLAGS) -I/usr/include/libxml2 -fpic $<


clean: 
	rm *.o ../libgpxparser.so

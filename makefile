CC = gcc
CFLAGS = -g -Wall
OBJECTS = myshell.c parse.c
TODELETE = myshell.o parse.o

myshell : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o myshell

.PHONY: clean
clean:
	rm -f myshell $(TODELETE)
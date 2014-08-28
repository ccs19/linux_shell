CC = c99
CFLAGS = -g -Wall
OBJECTS = myshell.c
TODELETE = myshell.o

myshell : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o myshell

.PHONY: clean
clean:
	rm -f myshell $(TODELETE)
#gcc -g -Wall myshell.c -o myshell >outFile.txt <inFile.txt &
CC=gcc
CFLAGS=

mysh: shell.c interpreter.c shellmemory.c
	$(CC) $(CFLAGS) -c shell.c interpreter.c shellmemory.c helpers.c
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o helpers.o

clean: 
	rm mysh; rm *.o

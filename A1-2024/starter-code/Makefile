CC=gcc
CFLAGS= -g

mysh: shell.c interpreter.c shellmemory.c pcb.c
	$(CC) $(CFLAGS) -c shell.c interpreter.c shellmemory.c helpers.c pcb.c
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o helpers.o pcb.o

clean: 
	rm mysh; rm *.o

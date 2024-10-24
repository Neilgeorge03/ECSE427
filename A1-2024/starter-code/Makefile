CC=gcc
CFLAGS= -g

mysh: shell.c interpreter.c shellmemory.c pcb.c scheduler.c
	$(CC) $(CFLAGS) -c shell.c interpreter.c shellmemory.c helpers.c pcb.c scheduler.c queue.c
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o helpers.o pcb.o scheduler.o queue.o

clean: 
	rm mysh; rm *.o

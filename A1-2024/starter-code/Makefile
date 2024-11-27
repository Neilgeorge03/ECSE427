CC = gcc
CFLAGS = -g
framesize ?= 18   # Default frame store size
varmemsize ?= 10  # Default variable store size

DEFINES = -D FRAME_STORE_SIZE=$(framesize) -D VARIABLE_STORE_SIZE=$(varmemsize)

mysh: shell.o interpreter.o shellmemory.o helpers.o pcb.o scheduler.o
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o helpers.o pcb.o scheduler.o

shell.o: shell.c
	$(CC) $(CFLAGS) $(DEFINES) -c shell.c

interpreter.o: interpreter.c
	$(CC) $(CFLAGS) $(DEFINES) -c interpreter.c

shellmemory.o: shellmemory.c
	$(CC) $(CFLAGS) $(DEFINES) -c shellmemory.c

pcb.o: pcb.c
	$(CC) $(CFLAGS) $(DEFINES) -c pcb.c

scheduler.o: scheduler.c
	$(CC) $(CFLAGS) $(DEFINES) -c scheduler.c

helpers.o: helpers.c
	$(CC) $(CFLAGS) $(DEFINES) -c helpers.c

clean: 
	rm mysh; rm *.o

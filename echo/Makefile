# Makefile for Proxy Lab

# Use $(CC) to invoke gcc
CC = gcc

# Compilation options:
# -O2: optimize level
# -Wall: display all warning messages
# -I .: add current directory to header file search path
CFLAGS = -O2 -Wall -I .

# -lpthread: links the POSIX thread library
LIB = -lpthread

# Target: Dependency
#     Command
echo: echoserveri.c echo.o csapp.o
	$(CC) $(CFLAGS) -o echoserveri echoserveri.c echo.o csapp.o $(LIB)

echo.o: echo.c
	$(CC) $(CFLAGS) -c echo.c

echoclient.o: echoclient.c csapp.o
	$(CC) $(CFLAGS) -o echoclient echoclient.c csapp.o $(LIB)

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm -f *.o echo *~
	rm -f echoserveri
	rm -f echoclient


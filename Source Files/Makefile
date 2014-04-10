CC=gcc
FLAGS=-W -Wall
LFLAGS=-pthread
OUTPUTFILENAME=assn2

main: main.o client.o server.o
	$(CC) $(LFLAGS) main.o client.o server.o -o $(OUTPUTFILENAME)

clean:
	rm -f *.o core.* $(OUTPUTFILENAME)

main.o: main.c
	$(CC) $(FLAGS) -c main.c

client.o: client.c
	$(CC) $(FLAGS) -c client.c

server.o: server.c
	$(CC) $(FLAGS) -c server.c
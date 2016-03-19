
CFLAGS=-Wall -g -std=c99
LIBS	= -lpthread
HEADERS=$(wildcard *.h)

all :: sem thread

sem: sem.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

thread: thread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -vf *.o


CC=clang

CFLAGS=-ggdb

PROG=__tests__/quadtree-test
OBJS=__tests__/quadtree-test.o quadtree.o llist.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ $(LIBS)

test: $(PROG)
	./$(PROG)

clean:
	rn -f $(PROG) $(OBJS)

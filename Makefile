CC=clang

CFLAGS=-ggdb

Q_PROG=__tests__/quadtree-test
Q_OBJS=__tests__/quadtree-test.o quadtree.o llist.o
L_PROG=__tests__/llist-test
L_OBJS=__tests__/llist-test.o llist.o

all: $(L_PROG) $(Q_PROG)

$(L_PROG): $(L_OBJS)
	$(CC) -o $(L_PROG) $(L_OBJS)

$(Q_PROG): $(Q_OBJS)
	$(CC) -o $(Q_PROG) $(Q_OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ $(LIBS)

test: $(L_PROG) $(Q_PROG)
	./$(L_PROG)
	./$(Q_PROG)

clean:
	rm -f $(L_PROG) $(Q_PROG) $(L_OBJS) $(Q_OBJS)


#CFLAGS=-I../src -g -Wall -O3 -lrt -lev
CFLAGS=-I../src -g -Wall -lrt -lev
LDLIBS=
CC=c99
VPATH=../src

NAMES=$(patsubst $(VPATH)/%.c, ./%.o, $(wildcard $(VPATH)/$(SNAME)*.c))
  # since wildcard doesn't care about VPATH...

webapp: webapp.o $(NAMES)
	$(CC) -lrt -lev webapp.o $(NAMES) -o webapp # grrr...

serve: webapp
	FGAJ_HOST=g FGAJ_LEVEL=10 ./webapp

vserve: webapp
	FGAJ_HOST=g FGAJ_LEVEL=10 valgrind --leak-check=full -v ./webapp
	#FGAJ_HOST=g FGAJ_LEVEL=10 valgrind --leak-check=full --show-reachable=yes -v ./webapp

clean:
	rm -f *.o
	rm -f *.so
	rm -f webapp

.PHONY: clean


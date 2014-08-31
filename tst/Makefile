
CFLAGS=-I../src -g -Wall -O3
LDLIBS=/usr/lib/libev.so # :-(
CC=c99
VPATH=../src

FLIBS=flutil gajeta aabro
SNAME=shv

NAMES=$(patsubst $(VPATH)/%.c, ./%.o, $(wildcard $(VPATH)/$(SNAME)_*.c))
  # since wildcard doesn't care about VPATH...

webapp: webapp.o shervin.o $(NAMES) $(FLIBS:=.o)

serve: webapp
	FGAJ_HOST=g FGAJ_LEVEL=10 ./webapp

vserve: webapp
	#FGAJ_HOST=g FGAJ_LEVEL=10 valgrind --leak-check=full -v ./webapp
	FGAJ_HOST=g FGAJ_LEVEL=10 valgrind --leak-check=full --show-reachable=yes -v ./webapp

clean:
	rm -f *.o
	rm -f *.so
	rm -f webapp

.PHONY: clean



CFLAGS=-I../src -g -Wall -O3
LDLIBS=/usr/lib/libev.so /usr/lib/x86_64-linux-gnu/libcurl.so # :-(
CC=c99
VPATH=../src

RODZO=$(shell which rodzo)
ifeq ($(RODZO),)
  RODZO=../../rodzo/bin/rodzo
endif

NAMES=$(patsubst $(VPATH)/%.c, ./%.o, $(wildcard $(VPATH)/$(SNAME)_*.c))
  # since wildcard doesn't care about VPATH...

s.c: ../spec/*_spec.c
	$(RODZO) ../spec -o s.c

s: $(NAME).o $(NAMES) $(FLIBS:=.o)

spec: s
	time ./s

vspec: s
	valgrind --leak-check=full -v ./s
	#valgrind --leak-check=full --show-reachable=yes -v ./s
	#valgrind --read-var-info=yes --track-origins=yes --leak-check=full -v ./s

dspec: s
	gdb ./s

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.c
	rm -f s

.PHONY: spec vspec gspec clean


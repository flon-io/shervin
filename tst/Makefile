
CFLAGS=-I../src -g -Wall -O3
LDLIBS=/usr/lib/libev.so # :-(
CC=c99
VPATH=../src

FLIBS=flutil aabro
SNAME=shv

NAMES=$(patsubst $(VPATH)/%.h, ./%.o, $(wildcard $(VPATH)/$(SNAME)_*.h))
  # since wildcard doesn't care about VPATH...

#s.c: ../spec/*_spec.c
#	$(RODZO) ../spec -o s.c

webapp: webapp.o shervin.o $(NAMES) $(FLIBS:=.o)

serve: webapp
	./webapp

#spec: s
#	time ./s
#
#vspec: s
#	valgrind --leak-check=full -v ./s

clean:
	rm -f *.o
	rm -f *.so
	rm -f webapp

.PHONY: clean


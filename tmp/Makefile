
#CFLAGS=-I../src -g -Wall -O3 -lrt -lev -lcurl
CFLAGS=-I../src -g -Wall -lrt -lev -lcurl
LDLIBS=
CC=c99
VPATH=../src

RODZO=$(shell which rodzo)
ifeq ($(RODZO),)
  RODZO=../../rodzo/bin/rodzo
endif

NAMES=$(patsubst $(VPATH)/%.c, ./%.o, $(wildcard $(VPATH)/*.c))
  # since wildcard doesn't care about VPATH...

s.c: ../spec/*_spec.c
	$(RODZO) ../spec -o s.c
	cp ../spec/*.[ch] .
	rm ./*_spec.c

servman.o: servman.c
bocla.o: bocla.c

s: $(NAMES) servman.o bocla.o

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
	rm -f *.[ch]
	rm -f s

.PHONY: spec vspec gspec clean


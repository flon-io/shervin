
NAME=shervin
SNAME=shv
FLIBS=flutil gajeta aabro

default: $(NAME).o

.DEFAULT spec clean:
	$(MAKE) -C tmp/ $@ NAME=$(NAME) SNAME=$(SNAME) FLIBS="$(FLIBS)"

# copy updated version of dep libs into src/
#
upgrade:
	cp -v ../gajeta/src/*.[ch] src/
	cp -v ../aabro/src/*.[ch] src/

cs: clean spec

.PHONY: spec clean upgrade cs


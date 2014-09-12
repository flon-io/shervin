
NAME=shervin

default: $(NAME).o

.DEFAULT spec clean:
	$(MAKE) -C tmp/ $@ NAME=$(NAME)

# copy updated version of dep libs into src/
#
upgrade:
	cp -v ../aabro/src/*.[ch] src/
	cp -v ../gajeta/src/*.[ch] src/
	cp -v ../flutil/src/*.[ch] src/

cs: clean spec

.PHONY: spec clean upgrade cs


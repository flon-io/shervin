
NAME=shervin
FLIBS=flutil aabro

#default: $(NAME).o
default: $(NAME)

.DEFAULT spec clean:
	$(MAKE) -C tmp/ $@ NAME=$(NAME) FLIBS="$(FLIBS)"

# copy updated version of dep libs into src/
#
upgrade:
	cp -v ../aabro/src/*.[ch] src/

cs: clean spec

.PHONY: spec clean upgrade cs


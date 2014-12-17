
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

cupgrade:
	cp -v ../bocla/src/bocla.[ch] spec/src/

.PHONY: spec clean upgrade cs



NAME=shervin

default: $(NAME).o

.DEFAULT spec clean:
	$(MAKE) -C tmp/ $@ NAME=$(NAME)

# copy updated version of dep libs into src/
#
stamp:
	cd $(REP) && git log -n 1 | sed 's/^/\/\//' >> ../$(NAME)/$(FIL)
upgrade:
	cp -v ../aabro/src/*.[ch] src/
	cp -v ../gajeta/src/*.[ch] src/
	cp -v ../flutil/src/*.[ch] src/
	find src/aabro.[ch] -exec $(MAKE) --quiet stamp REP=../aabro FIL={} \;
	find src/gajeta.[ch] -exec $(MAKE) --quiet stamp REP=../gajeta FIL={} \;
	find src/flutil.[ch] -exec $(MAKE) --quiet stamp REP=../flutil FIL={} \;

cupgrade:
	cp -v ../bocla/src/bocla.[ch] spec/src/

.PHONY: spec clean upgrade cs


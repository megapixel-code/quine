.PHONY: all clean
all: build/ out/quine
	out/quine
clean:
	-rm -r out/
	-rm -r build/

# testing quine
.PHONY: test
test: build/ build/quine2.c build/quine1.c
	diff build/quine1.c build/quine2.c

build/quine1.c: out/quine
	$< > $@
build/quine2.c: build/quine1
	$< > $@
# end testing quine

build/:
	mkdir -p build

build/quinify.c: quinify.c
	cp -f $< $@
build/quine.c: build/quinify main.c
	build/quinify main.c > build/quine.c

# from build/*.c to build/
build/%.o: build/%.c
	cc -c -o $@ $<
build/%: build/%.o
	cc -o $@ $<

out/quine: build/quine.o
	mkdir -p out
	cc -o $@ $<

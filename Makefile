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

build/lib.h.gch: src/lib.h
	cc -c $< -o $@
build/quinify.o: src/quinify.c
	cc -c -o $@ $<
build/quinify: build/quinify.o build/lib.h.gch
	cc -o $@ $< -Ibuild/

build/quine.c: build/quinify src/main.c src/lib.h
	build/quinify > build/quine.c

out/quine: build/quine.o out/
	cc -o $@ $<

# from build/*.c to build/
build/%.o: build/%.c
	cc -c -o $@ $<
build/%: build/%.o
	cc -o $@ $<

%/:
	mkdir -p $@

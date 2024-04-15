CC = gcc
CFLAGS = -I"./include" -Wall -Wextra

TESTSOURCE = test/json_parser_test.c
TESTTARGET = test/obj/test

.PHONY: build test clean

build: lib/json_parser.o

lib/json_parser.o: src/json_parser.c
	mkdir -p lib
	${CC} -o lib/json_parser.o -c src/json_parser.c ${CFLAGS}

${TESTTARGET}: ${TESTSOURCE}
	mkdir -p test/obj
	${CC} -o $@ $^ lib/json_parser.o ${CFLAGS} -lcriterion -L"./lib"
	$@

test: build ${TESTTARGET}
	echo $^ $< $@


clean:
	rm -r test/obj
	rm -r lib
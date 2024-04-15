CC = gcc
CFLAGS = -I"./include" -Wall -Wextra

TESTSOURCE = test/json_parser_test.c
TESTTARGET = test/obj/test

ASANOPTIONS = -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all \
	-fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment

.PHONY: build test clean example
.SILENT: example

clean:
	rm -rf test/obj
	rm -rf lib

build: lib/json_parser.o

lib/json_parser.o: src/json_parser.c
	mkdir -p lib
	${CC} -o lib/json_parser.o -c src/json_parser.c ${CFLAGS} -g3

${TESTTARGET}: ${TESTSOURCE}
	mkdir -p test/obj
	${CC} -o $@ $^ lib/json_parser.o ${CFLAGS} -lcriterion -L"./lib"
	$@ #--filter JSON_Parse_Object/* --debug & gdb -ex "target remote localhost:1234" -q ./test/obj/test

test: CFLAGS += -fno-omit-frame-pointer -g3 ${ASANOPTIONS}
test: build ${TESTTARGET}

example: build example/*.c
	mkdir -p example/obj
	${CC} -o example/obj/hello example/hello.c lib/json_parser.o ${CFLAGS}
	./example/obj/hello
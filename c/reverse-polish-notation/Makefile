CC = gcc
CFLAGS = -I"./include" -Wall -Wextra

TESTSOURCE = test/rpn_test.c
TESTTARGET = test/obj/test

ASANOPTIONS = -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all \
	-fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment

.PHONY: build test clean example
.SILENT: example

clean:
	rm -rf test/obj
	rm -rf lib

build: lib/rpn.o

lib/rpn.o: src/rpn.c
	mkdir -p lib
	${CC} -o lib/rpn.o -c src/rpn.c ${CFLAGS} -g3

${TESTTARGET}: ${TESTSOURCE}
	mkdir -p test/obj
	${CC} -o $@ $^ lib/rpn.o ${CFLAGS} -lcriterion -L"./lib"
	$@ ${TESTFLAGS} #--filter RPN_Literal/Negative --debug & gdb -ex "target remote localhost:1234" -q ./test/obj/test

test: CFLAGS += -fno-omit-frame-pointer -g3 ${ASANOPTIONS}
test: build ${TESTTARGET}

example: build example/*.c
	mkdir -p example/obj
	${CC} -o example/obj/hello example/hello.c lib/rpn.o ${CFLAGS}
	./example/obj/hello
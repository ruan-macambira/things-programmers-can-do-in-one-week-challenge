CC = gcc
CFLAGS = -I"./include" -Wall -Wextra

# only works when there is only one file in src/, probably
SRCFILENAME := $(shell ls src | sed -rn 's/^(\w+)\.c/\1/p')

TESTSOURCE = test/${SRCFILENAME}_test.c
TESTTARGET = test/obj/test

ASANOPTIONS = -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all \
	-fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment

.PHONY: build test clean example debug
.SILENT: example

clean:
	rm -rf test/obj
	rm -rf lib

build: lib/${SRCFILENAME}.o

lib/${SRCFILENAME}.o: src/${SRCFILENAME}.c
	mkdir -p lib
	${CC} -o lib/${SRCFILENAME}.o -c src/${SRCFILENAME}.c ${CFLAGS} -g3

${TESTTARGET}: ${TESTSOURCE}
	mkdir -p test/obj
	${CC} -o $@ $^ lib/${SRCFILENAME}.o ${CFLAGS} -lcriterion -L"./lib"
	$@ ${TESTFLAGS} ${DEBUGOPTIONS} #--filter Basic/Sum --debug & gdb -ex "target remote localhost:1234" -q ./${TESTTARGET}

test: CFLAGS += -fno-omit-frame-pointer -g3 ${ASANOPTIONS}
test: clean build ${TESTTARGET}
#test: DEBUGOPTIONS := --debug & gdb -ex "target remote localhost:1234" -q ./${TESTTARGET}
mkdir -p obj
gcc -o obj/json_parser.o -Wall -Wextra -c -g3 src/json_parser.c -I"./lib/"
gcc -Wall -Wextra -g -o obj/test test/json_parser_test.c obj/json_parser.o -I"./lib/" -lcriterion

if [[ $? -ne 0 ]]; then
    exit 1
fi

# Test file with isolated tests to debug when they have an error because I'm not sure it's possible to do this with criterion out of the box
gcc -Wall -Wextra -g -o obj/test-error test/json_parser_test_error.c obj/json_parser.o -I"./lib/" -lcriterion

./obj/test

if [[ $? -ne 0 ]]; then
    ./obj/test-error --debug=gdb &
    gdb -ex "target remote localhost:1234" -q ./obj/test-erro
fi
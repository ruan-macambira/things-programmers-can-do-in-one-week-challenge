mkdir -p bin
gcc -Wall -Wextra -g -o bin/test test/json_parser_test.c src/json_parser.c -I"./lib/" -lcriterion

./bin/test #--debug=gdb
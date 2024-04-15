# Reverse Polish Notation

[Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation) Calculator

## Building
```bash
$ make build
```

The binary will be generated at `lib/json_parser.o`

## Testing
```bash
$ make test
```

## Examples

```c
#include "rpn.h"

int main(void) {
    double number;
    bool success = RPN_tryParse("1 2 3 - +", &number);

    printf("1 2 3 - + = %lf", number); // 0
    return 0;
}
```
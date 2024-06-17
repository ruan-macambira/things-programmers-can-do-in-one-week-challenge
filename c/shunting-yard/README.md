# Shunting Yard Algorithm

Infix Notation Calculator implementing the [Shunting Yard Algorithm](https://en.wikipedia.org/wiki/Shunting_yard_algorithm)

## Building
```bash
$ make build
```

The binary will be generated at `lib/shunting_yard.o`

## Testing
```bash
$ make test
```

## Examples

```c
#include "shunting_yard.h"

int main(void) {
    double number;
    bool success = SH_trySolve("1 + 2 - 3", &number);

    printf("1 + 2 - 3 = %lf", number); // 0
    return 0;
}
```
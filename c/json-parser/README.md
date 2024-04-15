# C/JSON Parser

A barebones implementation of JSON parsing using Recursive Descent.

## Known Limitations
 - String Literals has limit of 255 characters. Beyond that is undefined behavior
 - Number Parsing is not prepared to handle numbers out of the scope of 64-bit float numbers
 - Number Parsing does not support `E`(e.g `1.5E-6`)
 - Does not support Unicode Characters escaping (e.g `\uXXXX`) beyond ASCII range

## Observations
 - "object" means, for the content of this text, the deserialized version of a valid JSON object, i.e, things like `true`, `false` and `null` counts as objects. Objects that are key-value pairs are referred as "JSON objects";
 - Every function that fetchs an element, with exception of number and boolean, returns a reference to the object instead of a copy, which means modifying it modifies the original object;
 - "get"-type functions does not check for the type of the object it is retrieving

Beyond these, this implementation is trying to adhere to [RFC 7159](https://datatracker.ietf.org/doc/html/rfc7159).

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
#include <stdio.h>
#include "json_parser.h"

int main(void) {
    // Parse the object and generate the Deserialized Object
    MyJSON_Object *json = MyJSON_parse("{\"input\": \"Hello World\n\"}");

    // If there is an error in the parsing, the object returns NULL
    if(json == NULL) {
        fprintf(stderr, "Parsing Error");
        return 1;
    }

    // Returns the type of the object (i.e if it's an array, string, etc.)
    if(MyJSON_type(json) != MyJSON_dict) {
        fprintf(stderr, "ERROR: unexpected type");
        return 1;
    }

    // Use for retrieving a value from JSON Objects with a known key
    MyJSON_Object *getInput = MyJSON_getDictElement(json, "input");
    
    // "MyJSON_getDictElement" returns a reference to an object, with its
    // actual content wrapped. Use "MyJSON_get(Boolean|Number|String) to
    // retrieve its raw value. WARNING: These functions to not check for
    // the type present in the object.
    const char *inputValue = MyJSON_getString(getInput);

    printf(inputValue);

    // Frees the Object. Only use in root objects (the one returned by "MyJSON_parse")
    MyJSON_free(json);

    return 0;
}
```
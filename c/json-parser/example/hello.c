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
#ifndef MYJSON_PARSER
#define MYJSON_PARSER

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

enum MyJSON_type {
    MyJSON_null,
    MyJSON_boolean,
    MyJSON_number,
    MyJSON_string,
    MyJSON_array,
    MyJSON_dict
};

typedef struct MyJSON_Dict {
    uint32_t size;
    char** keys;
    struct MyJSON_Object* values;
} MyJSON_Dict;

typedef struct MyJSON_Array {
    uint32_t size;
    struct MyJSON_Object *array;
} MyJSON_Array;

typedef struct MyJSON_Object {
    uint8_t type;
    struct {
        bool boolean;
        double number;
        char* string;
        MyJSON_Array* array;
        MyJSON_Dict* dict;
    };
} MyJSON_Object;

void MyJSON_free(MyJSON_Object *object);
void MyJSON_ignorewhitespace(const char* const, const char*);

static bool MyJSON_parseNull(const char *const serialized, MyJSON_Object *object, const char* endparse) {
    if(object->type != MyJSON_null) {
        return 0;
    }

    if(memcmp(serialized, "null", 4) == 0) {
        endparse = serialized + 4;
    }

    return endparse > serialized;
}

static bool MyJSON_parseBool(const char *const serialized, bool *boolean, const char* endparse) {
    if(memcmp(serialized, "true", 4) == 0) {
        endparse = serialized + 4;
        *boolean = true;
    } else if(memcmp(serialized, "false", 5) == 0) {
        endparse = serialized + 5;
        *boolean = false;
    }

    return endparse > serialized;
}

static bool MyJSON_parseNumber(const char* const serialized, double* number, const char* endparse) {
    //TODO: everything lol

    return endparse > serialized;
}

static bool MyJSON_parseString(const char* const serialized, char *object, const char* endparse) {
    return endparse > serialized;
}

static bool MyJSON_parseLiteral(const char* const serialized, MyJSON_Object *object, const char* endparse) {
    if(object->type != MyJSON_null) {
        return false;
    }

    if(MyJSON_parseNull(serialized, object, endparse)) {
        return true;
    }

    if(MyJSON_parseNumber(serialized, &(object->number), endparse)) {
        object->type = MyJSON_number;
        return true;
    }

    if(MyJSON_parseBool(serialized, &(object->boolean), endparse)) {
        object->type = MyJSON_boolean;
        return true;
    }

    if(MyJSON_parseString(serialized, (object->string), endparse)) {
        object->type = MyJSON_string;
        return true;
    }

    return false;
}

static bool MyJSON_parseArray(const char* const serialized, MyJSON_Object *object, const char* endparse) {
    if(object->type != MyJSON_null) {
        return 0;
    }

    if(*serialized != '[') {
        return 0;
    }

    object->type = MyJSON_array;
    object->array = malloc(sizeof(*(object->array)));

    const char* ptr = serialized + 1;
    while(*ptr != ']') {
        if(*ptr == '\0') {
            free(object->array);
            return 0;
        }

        ptr++;
    }
    endparse = ptr + 1;

    return endparse > serialized;
}

static bool MyJSON_parseDict(const char* const serialized, MyJSON_Object *object, const char* endparse) {
    if(object->type != MyJSON_null) {
        return 0;
    }

    if(*serialized != '{') {
        return 0;
    }

    object->type = MyJSON_dict;
    object->dict = malloc(sizeof(*(object->dict)));

    const char* ptr = serialized + 1;
    while(*ptr != '}') {
        if(*ptr == '\0') {
            free(object->dict);
            return 0;
        }

        ptr++;
    }
    endparse = ptr + 1;

    return endparse > serialized;
}

MyJSON_Object* MyJSON_parse(const char* const serialized) {
    MyJSON_Object *object = malloc(sizeof(*object));
    char* endpointer;
    bool result = (
        MyJSON_parseLiteral(serialized, object, endpointer) ||
        MyJSON_parseArray(serialized, object, endpointer) ||
        MyJSON_parseDict(serialized, object, endpointer)
    );

    if(!result) {
        return NULL;
    }

    return object;
}

#include <assert.h>
#include <stdio.h>
int main(void) {
    MyJSON_Object* object;

    object = MyJSON_parse("null");
    assert(object != NULL);
    assert(object->type == MyJSON_null);

    object = MyJSON_parse("true");
    assert(object != NULL);
    assert(object->type == MyJSON_boolean);
    assert(object->boolean == true);
    
    object = MyJSON_parse("false");
    assert(object != NULL);
    assert(object->type == MyJSON_boolean);
    assert(object->boolean == false);

    object = MyJSON_parse("{}");
    assert(object != NULL);
    assert(object->type == MyJSON_dict);
    assert(object->dict->size == 0);

    object = MyJSON_parse("[]");
    assert(object != NULL);
    assert(object->type == MyJSON_array);
    assert(object->array->size == 0);
}

#endif
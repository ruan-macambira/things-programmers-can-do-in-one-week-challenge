#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "json_parser.h"

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
    union {
        bool boolean;
        double number;
        char* string;
        MyJSON_Array* array;
        MyJSON_Dict* dict;

        void* any;
    };
} MyJSON_Object;

void MyJSON_free(MyJSON_Object *source) {
    switch(source->type) {
    case MyJSON_array:
        break;
    case MyJSON_dict:
        break;
    case MyJSON_string:
        break;
    default:
        break;
    }

    free(source);
}

enum MyJSON_type MyJSON_type(const MyJSON_Object *source) {
    return source->type;
}

bool MyJSON_isNull(const MyJSON_Object *source) {
    return source->type == MyJSON_null;
}

bool MyJSON_getBoolean(const MyJSON_Object *source) {
    return source->boolean;
}

double MyJSON_getNumber(const MyJSON_Object *source) {
    return source->number;
}

const char* MyJSON_getString(const MyJSON_Object *source) {
    return source->string;
}

size_t MyJSON_getArraySize(const MyJSON_Object *source) {
    return source->array->size;
}

MyJSON_Object* MyJSON_getArrayElement(MyJSON_Object *source, size_t index) {
    return source->array->array + index;
}

size_t MyJSON_getDictSize(const MyJSON_Object *source) {
    return source->dict->size;
}

MyJSON_Object* MyJSON_getDictElement(MyJSON_Object *source, const char *const key) {
    const size_t keylen = strlen(key);
    
    for(size_t i=0; i < source->dict->size; ++i) {
        const char* const keytest = source->dict->keys[i];
        if(strlen(keytest) != keylen) {
            continue;
        }
        if(memcmp(key, keytest, keylen) == 0) {
            return source->dict->values + i;
        }
    }

    return NULL;
}

void MyJSON_free(MyJSON_Object *object);
void MyJSON_ignorewhitespace(const char* const, const char*);
bool MyJSON_parseObject(const char* const, MyJSON_Object*, const char**);

static bool MyJSON_parseNull(const char *const serialized, MyJSON_Object *object, const char** endparse) {
    if(object->type != MyJSON_null) {
        return false;
    }

    if(memcmp(serialized, "null", 4) == 0) {
        *endparse = serialized + 4;
    }

    return *endparse > serialized;
}

static bool MyJSON_parseBool(const char *const serialized, bool *boolean, const char** endparse) {
    if(memcmp(serialized, "true", 4) == 0) {
        *endparse = serialized + 4;
        *boolean = true;
    } else if(memcmp(serialized, "false", 5) == 0) {
        *endparse = serialized + 5;
        *boolean = false;
    }

    return *endparse > serialized;
}

static bool MyJSON_parseNumber(const char* const serialized, double* number, const char** endparse) {
    *number = 0;

    return *endparse > serialized;
}

#define DEFAULT_STRING_SIZE 256
static bool MyJSON_parseString(const char* const serialized, char** string, const char** endparse) {
    if(serialized[0] != '\"') {
        return false;
    }

    *string = malloc(sizeof(**string) * DEFAULT_STRING_SIZE);
    size_t length = 0;

    bool escaped = false;
    const char *ptr = serialized + 1;
    while(*ptr != '\0') {
        if(*ptr == '\\') {
            escaped = true;
            ptr++;
            continue;
        }

        if(escaped) {
            switch(*ptr) {
            case '\\':
                break;
            default:
                break;
            }
            escaped = false;
            ptr++;
        }

        if(*ptr == '\"') {
            (*string)[length] = '\0';
            *endparse = ptr + 1;
            *string = realloc(*string, length+1);
            break;
        }

        (*string)[length++] = *ptr;
        ptr++;
    }
    return *endparse > serialized;
}

static bool MyJSON_parseLiteral(const char* const serialized, MyJSON_Object *object, const char** endparse) {
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

    if(MyJSON_parseString(serialized, &(object->string), endparse)) {
        object->type = MyJSON_string;
        return true;
    }

    return false;
}

static bool MyJSON_parseArray(const char* const serialized, MyJSON_Object *object, const char** endparse) {
    if(object->type != MyJSON_null) {
        return 0;
    }

    if(*serialized != '[') {
        return 0;
    }

    object->type = MyJSON_array;
    MyJSON_Array *array = calloc(1, sizeof(*array));
    array->array = calloc(5, sizeof(*(array->array)));
    object->array = array;

    const char* ptr = serialized + 1;
    while(*ptr != ']') {
        if(*ptr == '\0') {
            goto arrayParseError;
        }

        MyJSON_Object element = {};
        bool result = MyJSON_parseObject(ptr, &element, endparse);

        if(result) {
            array->array[array->size++] = element;
            ptr = *endparse;
        } else {
            goto arrayParseError;
        }

        if(*ptr != ']' && *ptr != ',') {
            goto arrayParseError;
        }

        if(*ptr == ',') ptr++;
    }
    *endparse = ptr + 1;

    return *endparse > serialized;

    arrayParseError:
        free(array->array);
        free(array);
        return 0;
}

static bool MyJSON_parseDict(const char* const serialized, MyJSON_Object *object, const char** endparse) {
    if(object->type != MyJSON_null) {
        return 0;
    }

    if(*serialized != '{') {
        return 0;
    }

    object->type = MyJSON_dict;
    object->dict = calloc(1, sizeof(*(object->dict)));

    const char* ptr = serialized + 1;
    while(*ptr != '}') {
        if(*ptr == '\0') {
            free(object->dict);
            return 0;
        }

        ptr++;
    }
    *endparse = ptr + 1;

    return *endparse > serialized;
}

bool MyJSON_parseObject(const char* const serialized, MyJSON_Object *object, const char** endparse) {
    return (
        MyJSON_parseLiteral(serialized, object, endparse) ||
        MyJSON_parseArray(serialized, object, endparse) ||
        MyJSON_parseDict(serialized, object, endparse)
    );
}

MyJSON_Object* MyJSON_parse(const char* const serialized) {
    MyJSON_Object *object = malloc(sizeof(*object));
    const char *endparse = NULL;

    bool result = MyJSON_parseObject(serialized, object, &endparse);

    if(!result) {
        return NULL;
    }

    return object;
}
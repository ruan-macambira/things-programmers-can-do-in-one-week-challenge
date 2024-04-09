#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "json_parser.h"

#define CONTAINER_DEFAULT_SIZE 5
#define CONTAINER_DEFAULT_GROWTH 1.5
#define DEFAULT_STRING_SIZE 256

typedef struct MyJSON_Dict {
    uint32_t size;
    uint32_t maxsize;
    struct MyJSON_DictElement *container;
} MyJSON_Dict;

typedef struct MyJSON_Array {
    uint32_t size;
    uint32_t maxsize;
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

typedef struct MyJSON_DictElement {
    char *key;
    MyJSON_Object value;
} MyJSON_DictElement;

static MyJSON_Array* MyJSON_ArrayInit(void) {
    MyJSON_Array *array = calloc(1, sizeof *array);
    array->maxsize = CONTAINER_DEFAULT_SIZE;
    array->array = calloc(CONTAINER_DEFAULT_SIZE, sizeof *(array->array));

    return array;
}

static MyJSON_Object* MyJSON_ArrayAllocate(MyJSON_Array *array) {
    if(array->size == array->maxsize) {
        uint32_t new_maxsize = (array->size) * CONTAINER_DEFAULT_GROWTH;
        array->maxsize = new_maxsize;
        array->array = realloc(array->array, sizeof(*(array->array)) * new_maxsize);
    }

    MyJSON_Object *allocated =  &(array->array[array->size++]);
    memset(allocated, 0, sizeof(*allocated));
    return allocated;
}

static void MyJSON_ArrayFree(MyJSON_Array *array) {
    free(array->array);
    free(array);
}

static MyJSON_Dict* MyJSON_DictInit(void) {
    MyJSON_Dict *dict = calloc(1, sizeof *dict);
    dict->maxsize = CONTAINER_DEFAULT_SIZE;
    dict->container = calloc(CONTAINER_DEFAULT_SIZE, sizeof *(dict->container));
    return dict;
}

static MyJSON_DictElement* MyJSON_DictAllocate(MyJSON_Dict *dict) {
    if(dict->size == dict->maxsize) {
        uint32_t new_maxsize = (dict->size) * CONTAINER_DEFAULT_GROWTH;
        dict->maxsize = new_maxsize;
        dict->container = realloc(dict->container, sizeof(*(dict->container)) * new_maxsize);
    }

    MyJSON_DictElement *allocated = &(dict->container[dict->size++]);
    memset(allocated, 0, sizeof(*allocated));
    return allocated;
}

static void MyJSON_DictFree(MyJSON_Dict *dict) {
    for(uint32_t i=0; i<dict->size; i++) {
        free(dict->container[i].key);
    }
    free(dict->container);
    free(dict);
}

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
        const char* const keytest = source->dict->container[i].key;
        if(strlen(keytest) != keylen) {
            continue;
        }
        if(memcmp(key, keytest, keylen) == 0) {
            return &(source->dict->container[i].value);
        }
    }

    return NULL;
}

bool MyJSON_parseObject(const char* const, MyJSON_Object*, const char**);

static bool MyJSON_ignorewhitespace(const char* const serialized, const char** endparse) {
    const char *ptr = serialized;
    while(*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n') {
        ptr++;
    }

    if(*ptr != '\0') {
        *endparse = ptr;
        return true;
    }

    return false;
}

static bool MyJSON_parseNull(const char *const serialized, MyJSON_Object *object, const char** endparse) {
    if(object->type != MyJSON_null) {
        return false;
    }

    const char *ptr = serialized;
    if(!MyJSON_ignorewhitespace(serialized, &ptr)) {
        return false;
    }

    if(memcmp(ptr, "null", 4) == 0) {
        *endparse = ptr + 4;
    }

    return *endparse > serialized;
}

static bool MyJSON_parseBool(const char *const serialized, bool *boolean, const char** endparse) {
    const char *ptr = serialized;
    if(!MyJSON_ignorewhitespace(serialized, &ptr)) {
        return false;
    }

    if(memcmp(ptr, "true", 4) == 0) {
        *endparse = ptr + 4;
        *boolean = true;
    } else if(memcmp(ptr, "false", 5) == 0) {
        *endparse = ptr + 5;
        *boolean = false;
    }

    return *endparse > serialized;
}

static bool MyJSON_parseNumber(const char* const serialized, double* number, const char** endparse) {
    double signal = 1;
    double total = 0;

    const char* ptr = serialized;
    if(!MyJSON_ignorewhitespace(serialized, &ptr)) {
        return false;
    }

    if(*ptr == '-') {
        signal = -1;
        ptr++;
    }

    if(*ptr == '0') {
        *number = 0;
        *endparse = ptr+1;
        return true;
    } else if(*ptr < '1' || *ptr > '9') {
        return false;
    }

    while(*ptr >= '0' && *ptr <= '9') {
        total *= 10;
        total += *ptr - '0';
        ptr++;
    }

    if(*ptr == '.') {
        ptr++;
        if(*ptr <= '0' || *ptr >= '9') {
            return false;
        }

        double power = 1;
        while(*ptr >= '0' && *ptr <= '9') {
            power /= 10;
            total += (*ptr - '0') * power;
            ptr++;
        }
    }

    total *= signal;
    *number = total;
    *endparse = ptr;
    return *endparse > serialized;
}

static bool MyJSON_parseString(const char* const serialized, char** string, const char** endparse) {
    const char *ptr = serialized;
    if(!MyJSON_ignorewhitespace(serialized, &ptr)) {
        return false;
    }

    if(*ptr != '\"') {
        return false;
    }

    *string = malloc(sizeof(**string) * DEFAULT_STRING_SIZE);
    size_t length = 0, unicode_i, unicode_chr;

    bool escaped = false, unicode = false;
    ++ptr;
    while(*ptr != '\0') {
        if(unicode) {
            if(++unicode_i > 4) {
                strncpy((*string) + length, (const char*)&unicode_chr, 1);
                *endparse = ptr + 1;
                *string = realloc(*string, length+1);
                goto stringParseEnd;
            }

            switch(*ptr) {
            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7':
            case '8': case '9':
                unicode_chr = (unicode_chr << 4) + (*ptr - '0');
                break;
            case 'A': case 'B': case 'C':
            case 'D': case 'E': case 'F':
                unicode_chr = (unicode_chr << 4) + (*ptr - 'A' + 10);
                break;
            default:
                goto stringParseError;
            }
            ptr++;
            continue;
        } else if(*ptr == '\\') {
            escaped = true;
            ptr++;
            continue;
        } else if(escaped) {
            switch(*ptr) {
            case 0x22:
            case 0x5c:
            case 0x2f:
            case 0x62:
            case 0x66:
            case 0x6e:
            case 0x72:
            case 0x74:
                break;
            case 0x75:
                unicode = true;
                unicode_i = 0;
                unicode_chr = 0;
                ptr++;
                continue;
            default:
                goto stringParseError;
                break;
            }
            escaped = false;
        } else if(*ptr == '\"') {
            (*string)[length] = '\0';
            *endparse = ptr + 1;
            *string = realloc(*string, length+1);
            break;
        }

        (*string)[length++] = *ptr;
        ptr++;
    }

    stringParseEnd:
        if(*endparse > serialized) {
            return true;
        }

    stringParseError:
        free(*string);
        *string = NULL;
        return false;
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

    const char *ptr = serialized;
    if(!MyJSON_ignorewhitespace(serialized, &ptr)) {
        return false;
    }

    if(*ptr != '[') {
        return 0;
    }

    object->type = MyJSON_array;
    MyJSON_Array *array = MyJSON_ArrayInit();
    object->array = array;
    ++ptr;

    const char *ptr_next;
    if(!MyJSON_ignorewhitespace(ptr, &ptr_next)) {
        goto arrayParseError;
    }
    ptr = ptr_next;

    while(*ptr != ']') {
        if(!MyJSON_ignorewhitespace(ptr, &ptr_next)) {
            goto arrayParseError;
        }
        ptr = ptr_next;

        if(*ptr == '\0') {
            goto arrayParseError;
        }

        MyJSON_Object* element =  MyJSON_ArrayAllocate(array);
        bool result = MyJSON_parseObject(ptr, element, endparse);

        if(result) {
            ptr = *endparse;
        } else {
            goto arrayParseError;
        }

        if(!MyJSON_ignorewhitespace(ptr, &ptr_next)) {
            goto arrayParseError;
        }
        ptr = ptr_next;

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

    const char *ptr = serialized;
    if(!MyJSON_ignorewhitespace(serialized, &ptr)) {
        return false;
    }

    if(*ptr != '{') {
        return 0;
    }

    object->type = MyJSON_dict;
    MyJSON_Dict *dict = MyJSON_DictInit();
    object->dict = dict;
    ++ptr;

    const char *ptr_next;
    if(!MyJSON_ignorewhitespace(ptr, &ptr_next)) {
        goto parseDictError;
    }

    ptr = ptr_next;
    while(*ptr != '}') {
        if(*ptr == '\0') {
            goto parseDictError;
        }

        MyJSON_DictElement *pair = MyJSON_DictAllocate(dict);
        bool key_success = MyJSON_parseString(ptr, &(pair->key), &ptr_next);
        if(!key_success) {
            goto parseDictError;
        }

        ptr = ptr_next;
        if(*ptr_next!= ':') {
            goto parseDictError;
        }
        ptr = ptr_next+1;

        bool value_success = MyJSON_parseObject(ptr, &(pair->value), &ptr_next);
        if(!value_success) {
            goto parseDictError;
        }
        ptr = ptr_next;

        if(*ptr != '}' && *ptr != ',') {
            goto parseDictError;
        }

        if(*ptr == ',') {
            ++ptr;
        }
    }
    *endparse = ptr + 1;

    return *endparse > serialized;

    parseDictError:
        free(dict->container);
        free(dict);
        return false;
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
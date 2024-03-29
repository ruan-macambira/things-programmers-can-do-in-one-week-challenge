#ifndef MYJSON_PARSER
#define MYJSON_PARSER

#include <stdbool.h>
#include <stddef.h>

typedef struct MyJSON_Object MyJSON_Object;
enum MyJSON_type {
    MyJSON_null,
    MyJSON_boolean,
    MyJSON_number,
    MyJSON_string,
    MyJSON_array,
    MyJSON_dict
};

MyJSON_Object* MyJSON_parse(const char* const serialized);
void MyJSON_free(MyJSON_Object* source);

enum MyJSON_type MyJSON_type(const MyJSON_Object* source);
bool MyJSON_isNull(const MyJSON_Object* source);
bool MyJSON_getBoolean(const MyJSON_Object* source);
double MyJSON_getNumber(const MyJSON_Object* source);
size_t MyJSON_getArraySize(const MyJSON_Object* source);
MyJSON_Object* MyJSON_getArrayElement(MyJSON_Object* source, size_t index);
size_t MyJSON_getDictSize(const MyJSON_Object* source);
MyJSON_Object* MyJSON_getDictElement(MyJSON_Object* source, const char* const key);
#endif
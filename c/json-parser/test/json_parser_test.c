#include <criterion/criterion.h>

#include "json_parser.h"

Test(JSON_Parse_SimpleLiteral, Null) {
    MyJSON_Object *object = MyJSON_parse("null");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_null, "%d != %d", MyJSON_type(object), MyJSON_null);

    MyJSON_free(object);
}

Test(JSON_Parse_SimpleLiteral, True) {
    MyJSON_Object *object = MyJSON_parse("true");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_boolean, "%d != %d", MyJSON_type(object), MyJSON_boolean);
    cr_assert_eq(MyJSON_getBoolean(object), true);

    MyJSON_free(object);
}

Test(JSON_Parse_String, EmptyString) {
    MyJSON_Object *object = MyJSON_parse("\"\"");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_string);
    cr_assert_str_empty(MyJSON_getString(object));

    MyJSON_free(object);
}

Test(JSON_Parse_String, NoEscapeString) {
    MyJSON_Object *object = MyJSON_parse("\"foobar\"");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_string);
    cr_assert_str_eq(MyJSON_getString(object), "foobar");

    MyJSON_free(object);
}

Test(JSON_Parse_String, EscapedString) {
    MyJSON_Object *object = MyJSON_parse("\"foobar\\\"\"");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_string);
    cr_assert_str_eq(MyJSON_getString(object), "foobar\"");

    MyJSON_free(object);
}

Test(JSON_Parse_String, EscapedUnicode) {
    MyJSON_Object *object = MyJSON_parse("\"foobar\\u0075x\"");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_string);
    cr_assert_str_eq(MyJSON_getString(object), "foobarux");

    MyJSON_free(object);
}

Test(JSON_Parse_Array, EmptyArray) {
    MyJSON_Object *object = MyJSON_parse("[    ]");
    cr_assert_not_null(object);
    cr_assert_eq(MyJSON_type(object), MyJSON_array, "%d != %d", MyJSON_type(object), MyJSON_array);

    MyJSON_free(object);
}

Test(JSON_Parse_Array, SingleElement) {
    MyJSON_Object *object = MyJSON_parse("[ true ]");
    cr_assert_not_null(object);

    size_t object_array_size = MyJSON_getArraySize(object);
    cr_assert_eq(MyJSON_type(object), MyJSON_array, "%d != %d", MyJSON_type(object), MyJSON_array);
    cr_assert_eq(object_array_size, 1);

    MyJSON_free(object);
}

Test(JSON_Parse_Array, TwoElements) {
    MyJSON_Object *object = MyJSON_parse("[ true , { } ]");
    cr_assert_not_null(object);

    size_t object_array_size = MyJSON_getArraySize(object);
    cr_assert_eq(MyJSON_type(object), MyJSON_array, "%d != %d", MyJSON_type(object), MyJSON_array);
    cr_assert_eq(object_array_size, 2);
    cr_assert_eq(MyJSON_type(MyJSON_getArrayElement(object, 1)), MyJSON_dict);

    MyJSON_free(object);
}

Test(JSON_Parse_Array, Grow) {
    MyJSON_Object *object = MyJSON_parse("[1, -1, 1.5, true, {}, [], \"foo\", null, false]");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_array, "%d != %d", MyJSON_type(object), MyJSON_array);
    cr_assert_eq(MyJSON_getArraySize(object), 9);

    MyJSON_free(object);
}

Test(JSON_Parse_Object, Empty) {
    MyJSON_Object *object = MyJSON_parse("{}");
    cr_assert_not_null(object);

    size_t object_dict_size = MyJSON_getDictSize(object);
    cr_assert_eq(MyJSON_type(object), MyJSON_dict, "%d != %d", MyJSON_type(object), MyJSON_dict);
    cr_assert_eq(object_dict_size, 0, "MyJSON_getDictSize: %d != %d", object_dict_size, 0);

    MyJSON_free(object);
}

Test(JSON_Parse_Object, OneKey) {
    MyJSON_Object *object = MyJSON_parse("{\"foo\": \"bar\"}");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_dict);
    MyJSON_Object *v = MyJSON_getDictElement(object, "foo");
    cr_assert_str_eq(MyJSON_getString(v), "bar");

    MyJSON_free(object);
}

Test(JSON_Parse_Number, Zero) {
    MyJSON_Object *object = MyJSON_parse("0");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_number);
    cr_assert_eq(MyJSON_getNumber(object), 0);

    MyJSON_free(object);
}

Test(JSON_Parse_Number, NonZero_Positive) {
    MyJSON_Object *object = MyJSON_parse("11");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_number);
    cr_assert_eq(MyJSON_getNumber(object), 11);

    MyJSON_free(object);
}

Test(JSON_Parse_Number, NonZero_Negative) {
    MyJSON_Object *object = MyJSON_parse("-11");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_number);
    cr_assert_eq(MyJSON_getNumber(object), -11);

    MyJSON_free(object);
}

Test(JSON_Parse_Number, Decimal_Positive) {
    MyJSON_Object *object = MyJSON_parse("1.5");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_number);
    cr_assert_eq(MyJSON_getNumber(object), 1.5);

    MyJSON_free(object);
}

Test(JSON_Parse_Number, Decimal_Negative) {
    MyJSON_Object *object = MyJSON_parse("-7.5");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_number);
    cr_assert_eq(MyJSON_getNumber(object), -7.5);

    MyJSON_free(object);
}

Test(JSON_Parse_General, Nesting) {
    MyJSON_Object *object = MyJSON_parse("[{\"foo\": 1, \"bar\": true}, \"testing:\\\"true\\\"\"]");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_array);
    cr_assert_eq(MyJSON_getArraySize(object), 2);

    MyJSON_Object *dict = MyJSON_getArrayElement(object, 0);
    cr_assert_eq(MyJSON_type(dict), MyJSON_dict);
    cr_assert_eq(MyJSON_getNumber(MyJSON_getDictElement(dict, "foo")), 1);
    cr_assert_eq(MyJSON_getBoolean(MyJSON_getDictElement(dict, "bar")), true);

    MyJSON_Object *str = MyJSON_getArrayElement(object, 1);
    cr_assert_eq(MyJSON_type(str), MyJSON_string);
    cr_assert_str_eq(MyJSON_getString(str), "testing:\"true\"");

    MyJSON_free(object);
}
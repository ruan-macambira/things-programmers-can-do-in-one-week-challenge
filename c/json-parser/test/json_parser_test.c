#include <criterion/criterion.h>

#include "json_parser.h"

Test(JSON_parser, Null) {
    MyJSON_Object *object = MyJSON_parse("null");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_null, "%d != %d", MyJSON_type(object), MyJSON_null);
}

Test(JSON_parser, True) {
    MyJSON_Object *object = MyJSON_parse("true");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_boolean, "%d != %d", MyJSON_type(object), MyJSON_boolean);
    cr_assert_eq(MyJSON_getBoolean(object), true);
}

Test(JSON_Parse_String, EmptyString) {
    MyJSON_Object *object = MyJSON_parse("\"\"");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_string);
    cr_assert_str_empty(MyJSON_getString(object));
}

Test(JSON_Parse_String, NoEscapeString) {
    MyJSON_Object *object = MyJSON_parse("\"foobar\"");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_string);
    cr_assert_str_eq(MyJSON_getString(object), "foobar");
}

Test(JSON_Parse_String, EscapedString) {
    MyJSON_Object *object = MyJSON_parse("\"foobar\\\"\"");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_string);
    cr_assert_str_eq(MyJSON_getString(object), "foobar\"");
}

Test(JSON_Parse_String, EscapedUnicode) {
    MyJSON_Object *object = MyJSON_parse("\"foobar\\u0075\"");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_string);
    cr_assert_str_eq(MyJSON_getString(object), "foobaru");
}

Test(JSON_Parse_Array, EmptyArray) {
    MyJSON_Object *object = MyJSON_parse("[]");
    cr_assert_not_null(object);
    cr_assert_eq(MyJSON_type(object), MyJSON_array, "%d != %d", MyJSON_type(object), MyJSON_array);
}

Test(JSON_Parse_Array, SingleElement) {
    MyJSON_Object *object = MyJSON_parse("[true]");
    cr_assert_not_null(object);

    size_t object_array_size = MyJSON_getArraySize(object);
    cr_assert_eq(MyJSON_type(object), MyJSON_array, "%d != %d", MyJSON_type(object), MyJSON_array);
    cr_assert_eq(object_array_size, 1);
}

Test(JSON_Parse_Array, TwoElements) {
    MyJSON_Object *object = MyJSON_parse("[true,{}]");
    cr_assert_not_null(object);

    size_t object_array_size = MyJSON_getArraySize(object);
    cr_assert_eq(MyJSON_type(object), MyJSON_array, "%d != %d", MyJSON_type(object), MyJSON_array);
    cr_assert_eq(object_array_size, 2);
    cr_assert_eq(MyJSON_type(MyJSON_getArrayElement(object, 1)), MyJSON_dict);
}

Test(JSON_Parse_Object, Empty) {
    MyJSON_Object *object = MyJSON_parse("{}");
    cr_assert_not_null(object);

    size_t object_dict_size = MyJSON_getDictSize(object);
    cr_assert_eq(MyJSON_type(object), MyJSON_dict, "%d != %d", MyJSON_type(object), MyJSON_dict);
    cr_assert_eq(object_dict_size, 0, "MyJSON_getDictSize: %d != %d", object_dict_size, 0);
}

Test(JSON_Parse_Object, OneKey) {
    MyJSON_Object *object = MyJSON_parse("{\"foo\":\"bar\"}");
    cr_assert_not_null(object);

    cr_assert_eq(MyJSON_type(object), MyJSON_dict);
    MyJSON_Object *v = MyJSON_getDictElement(object, "foo");
    cr_assert_str_eq(MyJSON_getString(v), "bar");
}
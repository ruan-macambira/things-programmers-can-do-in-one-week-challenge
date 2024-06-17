#include <criterion/criterion.h>
#include "shunting_yard.h"

Test(Fail, Operator_Only) {
    double number;
    cr_assert(!SH_trySolve("+", &number));
}

Test(Basic, Num) {
    double number;
    cr_assert(SH_trySolve("1", &number));
    cr_assert_eq(number, 1);
}

Test(Basic, Sum) {
    double number;
    cr_assert(SH_trySolve("1 + 2", &number));
    cr_assert_eq(number, 3);
}

// Test(Basic, Parenthesis) {
//     double number;
//     cr_assert(SH_trySolve("(1+2)", &number));
//     cr_assert_eq(number, 3);
// }

Test(Mix, Multiple_Precedences_1) {
    double number;
    cr_assert(SH_trySolve("2 + 3 * 3", &number));
    cr_assert_eq(number, 11.0, "expected %lf, got %lf", 11.0, number);
}

Test(Mix, Multiple_Precedences_2) {
    double number;
    cr_assert(SH_trySolve("1 - 2 * 3 + 4", &number));
    cr_assert_eq(number, -1.0, "expected %lf, got %lf", -1.0, number);
}

Test(Mix, Multiple_Precedences_3) {
    double number;
    cr_assert(SH_trySolve("1 + 10 / 5 - 3 * 1", &number));
    cr_assert_eq(number, 0.0, "expected %lf, got %lf", 0.0, number);
}
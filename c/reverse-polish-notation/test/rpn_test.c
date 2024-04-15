#include <criterion/criterion.h>
#include "rpn.h"

Test(Fail, Operator_Only) {
    double number;
    cr_assert(!RPN_trySolve("+", &number));
}

Test(Fail, Too_Many_Operators) {
    double number;
    cr_assert(!RPN_trySolve("2 +",&number));
}

Test(Fail, Too_Many_Numbers) {
    double number;
    cr_assert(!RPN_trySolve("1 2 3 +", &number));
}

Test(Literal, Zero) {
    double number;
    bool success = RPN_trySolve("0", &number);

    cr_assert(success);
    cr_assert_eq(number, 0.0);
}

Test(Literal, Negative) {
    double number;
    bool success = RPN_trySolve("-1", &number);

    cr_assert(success);
    cr_assert_eq(number, -1.0);
}

Test(Literal, Decimal) {
    double number;
    bool success = RPN_trySolve("1.5", &number);

    cr_assert(success);
    cr_assert_eq(number, 1.5);
}

Test(Expression, Basic) {
    double number;
    bool success = RPN_trySolve("5 0.5 *", &number);

    cr_assert(success);
    cr_assert_eq(number, 2.5);
}

// N: Number
// O: Operation

Test(Expression, NNNOO) {
    double number;
    bool success = RPN_trySolve("1 2 3 + -", &number);

    cr_assert(success);
    cr_assert_eq(number, -4);
}

Test(Expression, NNONO) {
    double number;
    bool success = RPN_trySolve("1 2 + 3 -", &number);

    cr_assert(success);
    cr_assert_eq(number, 0);
}

Test(Expression, NNONNOO) {
    double number;
    bool success = RPN_trySolve("1 2 + 3 4 - -", &number);

    cr_assert(success);
    cr_assert_eq(number, 4);
}

Test(Expression, Extreme) {
    double number;
    bool success = RPN_trySolve("1 1 1 1 1 1 1 1 1 1 1 * 1 1 1 1 1 1 1 1 1 / 1 1 * / * / / * * / / * / * * / * / * / *", &number);

    cr_assert(success);
    cr_assert_eq(number, 1);
}
#include "builtin.h"

lval* builtin_add(lenv* env, lval* node) { return builtin_op(env, node, '+'); }
lval* builtin_sub(lenv* env, lval* node) { return builtin_op(env, node, '-'); }
lval* builtin_mul(lenv* env, lval* node) { return builtin_op(env, node, '*'); }
lval* builtin_div(lenv* env, lval* node) { return builtin_op(env, node, '/'); }
lval* builtin_mod(lenv* env, lval* node) { return builtin_op(env, node, '%'); }

lval* builtin_op(lenv* env, lval* node, char op) {
    UNUSED(env);

    char name[2]; name[0] = op; name[1] = '\0';

    // Ensure all arguments are numbers
    for (int i = 0; i < node->count; i++) {
        LASSERT_ARG_TYPE(name, node, i, LVAL_NUM);
    }

    lval* x = lval_pop(node, 0);

    // If no arguments and op == '-', do unary negation
    if ((op == '-') && node->count == 0) {
        x->num *= -1;
    }

    while (node->count > 0) {
        lval* y = lval_pop(node, 0);

        if      (op == '+') { x->num += y->num; }
        else if (op == '-') { x->num -= y->num; }
        else if (op == '*') { x->num *= y->num; }
        else if (op == '/' || op == '%') {
            if (y->num == 0) {
                lval_del(x); lval_del(y);
                x = lval_err("Division by zero");
                break;
            }

            switch (op) {
                case '%': x->num = fmodf(x->num, y->num); break;
                case '/': x->num /= y->num; break;
            }
        }

        lval_del(y);
    }

    lval_del(node);
    return x;
}

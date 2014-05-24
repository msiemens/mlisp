#include <stdbool.h>
#include <math.h>

#include "eval.h"

//static inline bool is_int(float f) { return (f - (int) f == 0); }

lval* eval_sexpr(lval* node) {
    // Evaluate children
    for (int i = 0; i < node->count; i++) {
        node->values[i] = eval(node->values[i]);

        // Error checking
        if (node->values[i]->type == LVAL_ERR) {
            return lval_take(node, i);
        }
    }

    // Empty expression
    if (node->count == 0) { return node; }

    // Single expression
    else if (node->count == 1) { return lval_take(node, 0); }

    // Ensure first element is a symbol
    lval* op = lval_pop(node, 0);
    if (op->type != LVAL_SYM) {
        lval_del(op); lval_del(node);
        return lval_err("S-Expression doesn't start with a symbol!");
    }

    // Call builtin with operator
    lval* result = eval_builtin_op(node, op->sym[0]);
    lval_del(op);

    return result;
}

lval* eval(lval* node) {
    // Evaluate S-Expressions
    if (node->type == LVAL_SEXPR) {
        return eval_sexpr(node);
    }

    return node;
}

lval* eval_builtin_op(lval* node, char op) {
    // Ensure all arguments are numbers
    for (int i = 0; i < node->count; i++) {
        if (node->values[i]->type != LVAL_NUM) {
            lval_del(node);
            return lval_err("Cannot operate on non number!");
        }
    }

    lval* x = lval_pop(node, 0);

    // If no arguments and op == '-', do unary negation
    if ((op == '-') && node->count == 0) {
        x->num *= -1;
    }

    while (node->count > 0) {
        lval* y = lval_pop(node, 0);

        switch (op) {
            case '+': x->num += y->num; break;
            case '-': x->num -= y->num; break;
            case '*': x->num *= y->num; break;
            case '/':  // Fall through
            case '%':
                if (y->num == 0) {
                    lval_del(x); lval_del(y);
                    x = lval_err("Division by zero!");
                    goto exit_loop;  // Stop looping
                }

                switch (op) {
                    case '%': x->num = fmodf(x->num, y->num); break;
                    case '/': x->num /= y->num; break;
                }

                break;
            default:
                lval_del(x); lval_del(y);
                x = lval_err("Operator not implemented!");
                goto exit_loop;  // Stop looping
        }

        // y not used any more
        lval_del(y);
    }

    exit_loop: ;

    lval_del(node);

    return x;
}

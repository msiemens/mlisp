#include <stdbool.h>
#include <math.h>

#include "eval.h"

//static inline bool is_int(float f) { return (f - (int) f == 0); }

lval* eval_sexpr(lenv* env, lval* node) {
    // Evaluate children
    for (int i = 0; i < node->count; i++) {
        node->values[i] = eval(env, node->values[i]);

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
    lval* func = lval_pop(node, 0);
    if (func->type != LVAL_FUNC) {
        lval_del(func); lval_del(node);
        return lval_err("First element is not a function!");
    }

    // Call builtin with operator
    lval* result = func->func(env, node);
    lval_del(func);

    return result;
}

lval* eval(lenv* env, lval* node) {
    // Evaluate S-Expressions
    if (node->type == LVAL_SYM) {
        return lenv_get(env, node);
    } else if (node->type == LVAL_SEXPR) {
        return eval_sexpr(env, node);
    }

    return node;
}

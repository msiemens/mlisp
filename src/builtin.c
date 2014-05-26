#include "builtin.h"

void builtins_init(lenv* env) {
    // List functions
    builtin_create(env, builtin_list, "list");
    builtin_create(env, builtin_head, "head");
    builtin_create(env, builtin_tail, "tail");
    builtin_create(env, builtin_join, "join");
    builtin_create(env, builtin_eval, "eval");

    // Math functions
    builtin_create(env, builtin_add, "+"); builtin_create(env, builtin_sub, "-");
    builtin_create(env, builtin_mul, "*"); builtin_create(env, builtin_div, "/");
    builtin_create(env, builtin_mod, "%");

    builtin_create(env, builtin_def, "def");
}

void builtin_create(lenv* env, lbuiltin func, char* name) {
    lval* key = lval_sym(name);
    lval* value = lval_func(func);

    lenv_put(env, key, value);
    lval_del(key); lval_del(value);
}

lval* builtin_add(lenv* env, lval* node) { return builtin_op(env, node, '+'); }
lval* builtin_sub(lenv* env, lval* node) { return builtin_op(env, node, '-'); }
lval* builtin_mul(lenv* env, lval* node) { return builtin_op(env, node, '*'); }
lval* builtin_div(lenv* env, lval* node) { return builtin_op(env, node, '/'); }
lval* builtin_mod(lenv* env, lval* node) { return builtin_op(env, node, '%'); }

lval* builtin_op(lenv* env, lval* node, char op) {
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

    exit_loop:
    lval_del(node);
    return x;
}

lval* builtin_head(lenv* env, lval* node) {
    LASSERT_ARG_COUNT(node, 1, "head");
    LASSERT_ARG_TYPE(node, 0, LVAL_QEXPR, "head");
    LASSERT_ARG_NOT_EMPTY_LIST(node, 0, "head");

    lval* qexpr = lval_take(node, 0);

    // Delete all elements that are not head and return
    while (qexpr->count > 1) {
        lval_del(lval_pop(qexpr, 1));
    }

    return qexpr;
}

lval* builtin_tail(lenv* env, lval* node) {
    LASSERT_ARG_COUNT(node, 1, "tail");
    LASSERT_ARG_TYPE(node, 0, LVAL_QEXPR, "tail");
    LASSERT_ARG_NOT_EMPTY_LIST(node, 0, "tail");

    lval* qexpr = lval_take(node, 0);
    lval_del(lval_pop(qexpr, 0));

    return qexpr;
}

lval* builtin_list(lenv* env, lval* node) {
    node->type = LVAL_QEXPR;
    return node;
}

lval* builtin_eval(lenv* env, lval* node) {
    LASSERT_ARG_COUNT(node, 1, "eval");
    LASSERT_ARG_TYPE(node, 0, LVAL_QEXPR, "eval");

    lval* qexpr = lval_take(node, 0);
    qexpr->type = LVAL_SEXPR;

    return eval(env, qexpr);
}

lval* builtin_join(lenv* env, lval* node) {
    for (int i = 0; i < node->count; i++) {
        LASSERT_ARG_TYPE(node, i, LVAL_QEXPR, "join");
    }

    lval* qexpr = lval_pop(node, 0);
    while (node->count) {
        qexpr = lval_join(qexpr, lval_pop(node, 0));
    }

    lval_del(node);
    return qexpr;
}

lval* builtin_cons(lenv* env, lval* node) {
    LASSERT_ARG_COUNT(node, 2, "cons");
    LASSERT_ARG_TYPE(node, 1, LVAL_QEXPR, "cons");

    lval* value = lval_pop(node, 0);
    lval* list = lval_pop(node, 0);

    lval* container = lval_qexpr();
    container = lval_add(container, value);
    container = lval_join(container, list);

    lval_del(node);
    return container;
}

lval* builtin_def(lenv* env, lval* node) {
    LASSERT_ARG_TYPE(node, 0, LVAL_QEXPR, "def");

    lval* symbols = node->values[0];

    // Ensure all elements of first list are symbols
    for (int i = 0; i < symbols->count; i++) {
        LASSERT(node, (symbols->values[i]->type == LVAL_SYM), "Function \"def\" cannot define non-symbol!");
    }

    // Check for correct number of symbols and values
    LASSERT(node, (symbols->count == node->count - 1), "Number of values is not number of symbols!");

    // Assign copies of values to symbols
    for (int i = 0; i < symbols->count; i++) {
        lenv_put(env, symbols->values[i], node->values[i + 1]);
    }

    lval_del(node);
    return lval_sexpr();
}

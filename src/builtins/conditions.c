#include "builtin.h"

lval* builtin_gt(lenv* env, lval* node) { return builtin_ord(env, node, ">");  }
lval* builtin_ge(lenv* env, lval* node) { return builtin_ord(env, node, ">="); }
lval* builtin_lt(lenv* env, lval* node) { return builtin_ord(env, node, "<");  }
lval* builtin_le(lenv* env, lval* node) { return builtin_ord(env, node, "<="); }

lval* builtin_ord(lenv* env, lval* node, char* op) {
    UNUSED(env);

    // Check argument count
    LASSERT_MIN_ARG_COUNT(op, node, 2);

    // Ensure all arguments are numbers
    for (int i = 0; i < node->count; i++) {
        LASSERT_ARG_TYPE(op, node, i, LVAL_NUM);
    }

    lval* x = lval_pop(node, 0);
    PRECISION_INT result = 1;

    while (node->count > 0) {
        lval* y = lval_pop(node, 0);

        if      (strncmp(op, ">",  2) == 0) { result &= (x->num >  y->num); }
        else if (strncmp(op, ">=", 2) == 0) { result &= (x->num >= y->num); }
        else if (strncmp(op, "<",  2) == 0) { result &= (x->num <  y->num); }
        else if (strncmp(op, "<=", 2) == 0) { result &= (x->num <= y->num); }

        x->num = y->num;

        lval_del(y);
    }

    lval_del(node);

    x->num = result;
    return x;
}

lval* builtin_eq(lenv* env, lval* node) { return builtin_cmp(env, node, "=="); }
lval* builtin_ne(lenv* env, lval* node) { return builtin_cmp(env, node, "!="); }

lval* builtin_cmp(lenv* env, lval* node, char* op) {
    UNUSED(env);

    LASSERT_ARG_COUNT(op, node, 2);

    lval* o1 = node->values[0];
    lval* o2 = node->values[1];

    PRECISION_INT result = 0;

    if      (strcmp(op, "==") == 0) { result =  lval_eq(o1, o2); }
    else if (strcmp(op, "!=") == 0) { result = !lval_eq(o1, o2); }
    else                            { ASSERTF(0, "Invalid op in builtin_cmp: %s", op); }

    lval_del(node);
    return lval_num(result);
}

lval* builtin_and(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("and", node, 2);
    LASSERT_ARG_TYPE("and", node, 0, LVAL_NUM);
    LASSERT_ARG_TYPE("and", node, 1, LVAL_NUM);

    PRECISION_INT result = node->values[0]->num && node->values[1]->num;
    lval_del(node);

    return lval_num(result);
}

lval* builtin_or(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("or", node, 2);
    LASSERT_ARG_TYPE("or", node, 0, LVAL_NUM);
    LASSERT_ARG_TYPE("or", node, 1, LVAL_NUM);

    PRECISION_INT result = node->values[0]->num || node->values[1]->num;
    lval_del(node);

    return lval_num(result);
}

lval* builtin_not(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("not", node, 1);
    LASSERT_ARG_TYPE("not", node, 0, LVAL_NUM);

    PRECISION_INT result = !node->values[0]->num;
    lval_del(node);

    return lval_num(result);
}

lval* builtin_if(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_MIN_ARG_COUNT("if", node, 2);
    LASSERT_MAX_ARG_COUNT("if", node, 3);
    LASSERT_ARG_TYPE("if", node, 0, LVAL_NUM);
    LASSERT_ARG_TYPE("if", node, 1, LVAL_QEXPR);

    if (node->count == 3) {
        LASSERT_ARG_TYPE("if", node, 2, LVAL_QEXPR);
    }

    lval* branch;

    // Check condition and choose branch
    if (node->values[0]->num) {
        branch = lval_pop(node, 1);
    } else if (node->count == 3) {
        branch = lval_pop(node, 2);
    } else {
        lval_del(node);
        return lval_sexpr();
    }

    lval_del(node);

    branch->type = LVAL_SEXPR;
    return eval(env, branch);
}

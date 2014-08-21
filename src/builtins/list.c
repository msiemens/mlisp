#include "builtin.h"

lval* builtin_head(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("head", node, 1);
    LASSERT_ARG_TYPE("head", node, 0, LVAL_QEXPR);
    LASSERT_ARG_NOT_EMPTY_LIST("head", node, 0);

    lval* qexpr = lval_take(node, 0);

    // Delete all elements that are not head and return
    while (qexpr->count > 1) {
        lval_del(lval_pop(qexpr, 1));
    }

    return qexpr;
}

lval* builtin_tail(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("tail", node, 1);
    LASSERT_ARG_TYPE("tail", node, 0, LVAL_QEXPR);
    LASSERT_ARG_NOT_EMPTY_LIST("tail", node, 0);

    lval* qexpr = lval_take(node, 0);
    lval_del(lval_pop(qexpr, 0));

    return qexpr;
}

lval* builtin_list(lenv* env, lval* node) {
    UNUSED(env);

    node->type = LVAL_QEXPR;
    return node;
}

lval* builtin_join(lenv* env, lval* node) {
    UNUSED(env);

    for_item(node, {
        LASSERT_ARG_TYPE("join", node, i, LVAL_QEXPR);
    });

    lval* qexpr = lval_pop(node, 0);
    while (node->count) {
        qexpr = lval_join(qexpr, lval_pop(node, 0));
    }

    lval_del(node);
    return qexpr;
}

lval* builtin_cons(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("cons", node, 2);
    LASSERT_ARG_TYPE("cons", node, 1, LVAL_QEXPR);

    lval* value = lval_pop(node, 0);
    lval* list = lval_pop(node, 0);

    lval* container = lval_qexpr();
    container = lval_add(container, value);
    container = lval_join(container, list);

    lval_del(node);
    return container;
}

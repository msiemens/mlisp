#include "builtin.h"

lval* builtin_def(lenv* env, lval* node) { return builtin_var(env, node, DEF_GLOBAL); }
lval* builtin_put(lenv* env, lval* node) { return builtin_var(env, node, DEF_LOCAL); }

lval* builtin_var(lenv* env, lval* node, def_scope type) {
    LASSERT_ARG_TYPE((type == DEF_GLOBAL) ? "def" : "=", node, 0, LVAL_QEXPR);

    lval* symbols = node->values[0];

    // Ensure all elements of first list are symbols and not builtins
    // TODO: Make more beautiful
    for_item(symbols, {
        LASSERT(node, (item->type == LVAL_SYM),
                "Function \"def\" cannot define non-symbol. Expected %s, got %s.",
                lval_str_type(LVAL_SYM), lval_str_type(item->type));
    });

    // Check for correct number of symbols and values
    LASSERT(node, (symbols->count == node->count - 1),
            "Number of values is not number of symbols!");

    // Assign copies of values to symbols
    for_item(symbols, {
        switch (type) {
            case DEF_LOCAL:  lenv_put(env, item, node->values[i + 1]);
                break;
            case DEF_GLOBAL: lenv_def(env, item, node->values[i + 1]);
                break;
            default:  ASSERTF(0, "Invalid def_type passed: %i", type);
        }
    });

    lval_del(node);
    return lval_sexpr();
}

lval* builtin_lambda(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("lambda", node, 2);
    LASSERT_ARG_TYPE("lambda", node, 0, LVAL_QEXPR);
    LASSERT_ARG_TYPE("lambda", node, 1, LVAL_QEXPR);

    // Check if first Q-Expression only contains symbols
    lval* first_value = node->values[0];
    for_item(first_value, {
        LASSERT(node, (item->type == LVAL_SYM),
                "Cannot define non-symbol. Expected %s, got %s.",
                lval_str_type(LVAL_SYM), lval_str_type(item->type));
    })

    lval* formals = lval_pop(node, 0);
    lval* body    = lval_pop(node, 0);
    lval_del(node);

    return lval_lambda(formals, body);
}

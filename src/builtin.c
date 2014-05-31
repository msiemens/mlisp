#include "builtin.h"

// TODO: Too easy to forget to update, make dynamic?
const char* BUILTINS[] = {"+", "-", "*", "/", "%", "head", "tail", "list",
                         "eval", "join", "cons", "def", "=", "lambda"};

bool is_builtin(char* name) {
    for (unsigned int i = 0; i < sizeof(BUILTINS) / sizeof(*BUILTINS); ++i) {
        if (strcmp(name, BUILTINS[i]) == 0) {
            return true;
        }
    }
    return false;
}

void builtins_init(lenv* env) {
    builtin_create(env, builtin_eval, "eval");

    // List functions
    builtin_create(env, builtin_list, "list");
    builtin_create(env, builtin_head, "head");
    builtin_create(env, builtin_tail, "tail");
    builtin_create(env, builtin_join, "join");
    builtin_create(env, builtin_cons, "cons");

    // Math functions
    builtin_create(env, builtin_add, "+"); builtin_create(env, builtin_sub, "-");
    builtin_create(env, builtin_mul, "*"); builtin_create(env, builtin_div, "/");
    builtin_create(env, builtin_mod, "%");

    builtin_create(env, builtin_lambda, "lambda");
    builtin_create(env, builtin_def, "def");
    builtin_create(env, builtin_put, "=");
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
    UNUSED(env);

    char name[2]; name[0] = op; name[1] = '\0';

    // Ensure all arguments are numbers
    for (int i = 0; i < node->count; i++) {
        LASSERT_ARG_TYPE(node, i, LVAL_NUM, name);
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

        // y not used any more
        lval_del(y);
    }

    lval_del(node);
    return x;
}

lval* builtin_head(lenv* env, lval* node) {
    UNUSED(env);

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
    UNUSED(env);

    LASSERT_ARG_COUNT(node, 1, "tail");
    LASSERT_ARG_TYPE(node, 0, LVAL_QEXPR, "tail");
    LASSERT_ARG_NOT_EMPTY_LIST(node, 0, "tail");

    lval* qexpr = lval_take(node, 0);
    lval_del(lval_pop(qexpr, 0));

    return qexpr;
}

lval* builtin_list(lenv* env, lval* node) {
    UNUSED(env);

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
    UNUSED(env);

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
    UNUSED(env);

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

lval* builtin_def(lenv* env, lval* node) { return builtin_var(env, node, DEF_GLOBAL); }
lval* builtin_put(lenv* env, lval* node) { return builtin_var(env, node, DEF_LOCAL); }

lval* builtin_var(lenv* env, lval* node, def_type type) {
    LASSERT_ARG_TYPE(node, 0, LVAL_QEXPR, (type == DEF_GLOBAL) ? "def" : "=");

    lval* symbols = node->values[0];

    // Ensure all elements of first list are symbols and not builtins
    for (int i = 0; i < symbols->count; i++) {
        LASSERT(node, (symbols->values[i]->type == LVAL_SYM),
               "Function \"def\" cannot define non-symbol. Expected %s, got %s.",
               lval_str_type(LVAL_SYM), lval_str_type(symbols->values[i]->type));
        LASSERT(node, !is_builtin(symbols->values[i]->sym),
                "Cannot redefine builtin '%s'.", symbols->values[i]->sym);
    }

    // Check for correct number of symbols and values
    LASSERT(node, (symbols->count == node->count - 1),
            "Number of values is not number of symbols!");

    // Assign copies of values to symbols
    for (int i = 0; i < symbols->count; i++) {
        switch (type) {
            case DEF_LOCAL:  lenv_put(env, symbols->values[i], node->values[i + 1]); break;
            case DEF_GLOBAL: lenv_def(env, symbols->values[i], node->values[i + 1]); break;
            default: assertf(0, "Invalid def_type passed: %i", type);
        }
    }

    lval_del(node);
    return lval_sexpr();
}

lval* builtin_lambda(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT(node, 2, "lambda");
    LASSERT_ARG_TYPE(node, 0, LVAL_QEXPR, "lambda");
    LASSERT_ARG_TYPE(node, 1, LVAL_QEXPR, "lambda");

    // Check if first Q-Expression only contains symbols
    for (int i = 0; i < node->values[0]->count; i++) {
        LASSERT(node, (node->values[0]->values[i]->type == LVAL_SYM),
               "Cannot define non-symbol. Expected %s, got %s.",
               lval_str_type(LVAL_SYM), lval_str_type(node->values[0]->values[i]->type));
    }

    lval* formals = lval_pop(node, 0);
    lval* body    = lval_pop(node, 0);
    lval_del(node);

    return lval_lambda(formals, body);
}

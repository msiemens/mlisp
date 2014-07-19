#include "builtin.h"

void builtins_init(lenv* env) {
    builtin_create(env, builtin_eval,    "eval");
    builtin_create(env, builtin_load,    "load");
    builtin_create(env, builtin_print,   "print");
    builtin_create(env, builtin_println, "println");
    builtin_create(env, builtin_repr,    "repr");
    builtin_create(env, builtin_error,   "error");

    // List functions
    builtin_create(env, builtin_list, "list");
    builtin_create(env, builtin_head, "head");
    builtin_create(env, builtin_tail, "tail");
    builtin_create(env, builtin_join, "join");
    builtin_create(env, builtin_cons, "cons");

    // Math functions
    builtin_create(env, builtin_add, "+");
    builtin_create(env, builtin_sub, "-");
    builtin_create(env, builtin_mul, "*");
    builtin_create(env, builtin_div, "/");
    builtin_create(env, builtin_mod, "%");

    // Conditions
    builtin_create(env, builtin_gt, ">");
    builtin_create(env, builtin_ge, ">=");
    builtin_create(env, builtin_lt, "<");
    builtin_create(env, builtin_le, "<=");
    builtin_create(env, builtin_eq, "==");
    builtin_create(env, builtin_ne, "!=");
    builtin_create(env, builtin_and, "and");
    builtin_create(env, builtin_or, "or");
    builtin_create(env, builtin_not, "not");
    builtin_create(env, builtin_if, "if");

    // Functions and scopes
    builtin_create(env, builtin_lambda, "lambda");
    builtin_create(env, builtin_def, "def");
    builtin_create(env, builtin_put, "=");

    DEBUG_ONLY(
        builtin_create(env, builtin_debug_stats, "debug_stats");
    )
}

void builtin_create(lenv* env, lbuiltin func, char* name) {
    lval* key = lval_sym(name);
    lval* value = lval_func(func);

    lenv_put(env, key, value);

    lval_del(key); lval_del(value);
}

#pragma once

#include "stdbool.h"

#include "lval.h"
#include "eval.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#define LERROR(node, fmt, ...) __extension__({ \
    lval* error = lval_err(fmt, ##__VA_ARGS__); \
    lval_del(node); \
    return error;\
})

#define LASSERT(node, cond, fmt, ...) __extension__({ \
    if (!(cond)) { \
        LERROR(node, fmt, ##__VA_ARGS__); \
    } \
})

#define LASSERT_ARG_COUNT(node, ecount, name) \
    if (node->count > ecount) { \
        LERROR(node, "Function '%s' passed too many arguments. Expected %i, got %i.", \
               name, ecount, node->count); \
    } else if (node->count < ecount) { \
        LERROR(node, "Function '%s' passed too few arguments. Expected %i, got %i.", \
               name, ecount, node->count); \
    }

#define LASSERT_ARG_TYPE(node, i, etype, name) \
    LASSERT(node, node->values[i]->type == etype, \
            "Function '%s' passed incorrect argument types. Expected %s, got %s.", \
            name, lval_str_type(etype), lval_str_type(node->values[i]->type));

#define LASSERT_ARG_NOT_EMPTY_LIST(node, i, name) \
    LASSERT(node, node->values[i]->count > 0, \
            "Function '%s' passed empty list.", name);

#pragma GCC diagnostic pop


typedef enum def_type { DEF_LOCAL, DEF_GLOBAL } def_type;


bool is_builtin(char* name);

void builtins_init(lenv* env);

void builtin_create(lenv* env, lbuiltin func, char* name);


lval* builtin_add(lenv* env, lval* node);

lval* builtin_sub(lenv* env, lval* node);

lval* builtin_mul(lenv* env, lval* node);

lval* builtin_div(lenv* env, lval* node);

lval* builtin_mod(lenv* env, lval* node);

lval* builtin_op(lenv* env, lval* node, char op);

lval* builtin_head(lenv* env, lval* node);

lval* builtin_tail(lenv* env, lval* node);

lval* builtin_list(lenv* env, lval* node);

lval* builtin_eval(lenv* env, lval* node);

lval* builtin_join(lenv* env, lval* node);

lval* builtin_cons(lenv* env, lval* node);

lval* builtin_def(lenv* env, lval* node);

lval* builtin_put(lenv* env, lval* node);

lval* builtin_var(lenv* env, lval* node, def_type type);

lval* builtin_lambda(lenv* env, lval* node);

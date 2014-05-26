#pragma once

#include "lval.h"
#include "eval.h"

#define LERROR(node, msg) lval_del(node); return lval_err(msg);
#define LASSERT(node, cond, err) if (!(cond)) { LERROR(node, err); }
#define LASSERT_ARG_COUNT(node, ecount, name) \
if (node->count > ecount) { \
    LERROR(node, "Function " #name " passed too many arguments!"); \
} else if (node->count < ecount) { \
    LERROR(node, "Function " #name " passed too few arguments!") \
}
#define LASSERT_ARG_TYPE(node, i, etype, name) \
if (!(node->values[i]->type == etype)) { \
    LERROR(node, "Function " #name " passed incorrect argument types!"); \
}
#define LASSERT_ARG_NOT_EMPTY_LIST(node, i, name) \
if (node->values[i]->count == 0) { \
    LERROR(node, "Function " #name " passed empty list!"); \
}


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

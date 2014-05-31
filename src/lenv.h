#pragma once

#include "utils.h"
#include "lval.h"

#define LENV_GET(env, i) \
    env->values[i]

#define NEW_LENV()\
    xmalloc(LENV_SIZE)

#define LENV_SIZE sizeof(lenv)
#define LENV_PTR_SIZE sizeof(lenv*)
#define CHAR_PTR_SIZE sizeof(char*)

//! Environment container
typedef struct lenv {
    lenv* parent;

    int count;
    char** symbols;
    lval** values;
} lenv;


// Constructor & destructor
lenv* lenv_new(void);

void lenv_del(lenv* env);

lenv* lenv_copy(lenv* env);

// Get & set
lval* lenv_get(lenv* env, lval* key);

void lenv_put(lenv* env, lval* key, lval* value);

void lenv_def(lenv* env, lval* key, lval* value);

char* lenv_func_name(lenv* env, lbuiltin func);

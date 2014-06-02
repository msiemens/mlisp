#pragma once

#include "utils.h"
#include "lval.h"


//! Environment container
typedef struct lenv {
    lenv* parent;

    int count;
    char** symbols;
    lval** values;
} lenv;


static const size_t LENV_SIZE     = sizeof(lenv);
static const size_t LENV_PTR_SIZE = sizeof(lenv*);
static const size_t CHAR_PTR_SIZE = sizeof(char*);


// Constructor & destructor
lenv* lenv_new(void);

void lenv_del(lenv* env);

lenv* lenv_copy(lenv* env);

// Get & set
lval* lenv_get(lenv* env, lval* key);

void lenv_put(lenv* env, lval* key, lval* value);

void lenv_def(lenv* env, lval* key, lval* value);

char* lenv_func_name(lenv* env, lbuiltin func);

#pragma once

#include "lval.h"

#define NEW_LENV malloc(LENV_SIZE)
#define LENV_SIZE sizeof(lenv)
#define LENV_PTR_SIZE sizeof(lenv*)
#define CHAR_PTR_SIZE sizeof(char*)

//! Environment container
typedef struct lenv {
    int count;
    char** symbols;
    lval** values;
} lenv;


// Constructor & destructor
lenv* lenv_new(void);

void lenv_del(lenv* env);

// Get & set
lval* lenv_get(lenv* env, lval* key);

void lenv_put(lenv* env, lval* key, lval* value);

#pragma once

#include "mpc.h"

#include "config.h"

#define NEW_LVAL malloc(LVAL_SIZE)
#define LVAL_SIZE sizeof(lval)
#define LVAL_PTR_SIZE sizeof(lval*)

//! Value container
typedef struct lval {
    int type;

    // Data
    union {
        PRECISION num;  // Number types
        char* err;  // Error type
        char* sym;  // Symbol type
    };

    // S-Expr: values
    int count;
    struct lval** values;
} lval;

//! Value types
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };


lval* lval_num(PRECISION value);

lval* lval_err(char* message);

lval* lval_sym(char* symbol);

lval* lval_sexpr(void);

lval* lval_add(lval* container, lval* value);

lval* lval_pop(lval* node, int index);

lval* lval_take(lval* node, int index);

lval* lval_read_num(mpc_ast_t* tree);

lval* lval_read(mpc_ast_t* tree);

void lval_del(lval* node);

void lval_expr_print(lval* node, char open, char close);

void lval_print(lval* node);

void lval_println(lval* node);

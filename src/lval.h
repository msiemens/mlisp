#pragma once

#include "mpc.h"

#include "config.h"

// TODO: Better place
#define STRCPY(src, dst) malloc(strlen(src) + 1); strcpy(dst, src);
#define RSTRCPY(src, dst) realloc(dst, strlen(src) + 1); strcpy(dst, src);

#define NEW_LVAL malloc(LVAL_SIZE)
#define LVAL_SIZE sizeof(lval)
#define LVAL_PTR_SIZE sizeof(lval*)

// Forward declarations
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

//! Pointer to builtin function
typedef lval*(*lbuiltin)(lenv*, lval*);

//! Value container
struct lval {
    int type;

    // Data
    union {
        PRECISION num;  // Number type
        char* err;  // Error type
        char* sym;  // Symbol type

        struct {
            // S-Expr: values
            int count;
            struct lval** values;
        };

        lbuiltin func;  // Builtin type
    };

};

//! Value types
enum { LVAL_SEXPR, LVAL_QEXPR, LVAL_SYM, LVAL_FUNC, LVAL_NUM, LVAL_ERR };

// Constructors & destructor
lval* lval_sexpr(void);

lval* lval_qexpr(void);

lval* lval_sym(char* symbol);

lval* lval_num(PRECISION value);

lval* lval_err(char* message);

lval* lval_func(lbuiltin func);

void lval_del(lval* node);

lval* lval_copy(lval* node);

// Lvalue modifiers
lval* lval_add(lval* container, lval* value);

lval* lval_join(lval* first, lval* second);

lval* lval_pop(lval* node, int index);

lval* lval_take(lval* node, int index);

// Lvalue parsers
lval* lval_read_num(mpc_ast_t* tree);

lval* lval_read(mpc_ast_t* tree);

// Printers
void lval_expr_print(lval* node, char open, char close);

void lval_print(lval* node);

void lval_println(lval* node);

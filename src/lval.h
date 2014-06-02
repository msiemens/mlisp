#pragma once

#if !defined(MLISP_NOINCLUDE)
    #include "mpc.h"
#endif

#include "config.h"
#include "utils.h"

#define STR_BUFFER_SIZE 512


// Forward declarations
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

#if !defined(MLISP_NOINCLUDE)
    lenv* lenv_new(void);
    void lenv_del(lenv* env);
    lenv* lenv_copy(lenv* env);
#endif

//! Pointer to builtin function
typedef lval*(*lbuiltin)(lenv*, lval*);

//! Value types
typedef enum lval_type { LVAL_SEXPR, LVAL_QEXPR, LVAL_SYM, LVAL_FUNC,
                         LVAL_NUM, LVAL_ERR } lval_type;

//! Value container
struct lval {
    lval_type type;

    // Data
    union {
        PRECISION num;  // Number type
        char* err;      // Error type
        char* sym;      // Symbol type

        struct {
            // S-Expr: values
            int count;
            struct lval** values;
        };

        struct {
            lbuiltin builtin;  // Builtin func OR ...
            lenv* env;         // Lambda
            lval* formals;
            lval* body;
        };
    };

};


static const size_t LVAL_SIZE     = sizeof(lval);
static const size_t LVAL_PTR_SIZE = sizeof(lval*);

// Constructors & destructor
lval* lval_new(void);

lval* lval_sexpr(void);

lval* lval_qexpr(void);

lval* lval_sym(char* symbol);

lval* lval_num(PRECISION value);

lval* lval_err(char* fmt, ...);

lval* lval_func(lbuiltin func);

lval* lval_lambda(lval* formals, lval* body);

void lval_del(lval* node);

lval* lval_copy(lval* node);

int lval_eq(lval* x, lval* y);

// Lvalue modifiers
lval* lval_add(lval* container, lval* value);

lval* lval_join(lval* first, lval* second);

lval* lval_pop(lval* node, int index);

lval* lval_take(lval* node, int index);

// Lvalue parsers
lval* lval_read_num(mpc_ast_t* tree);

lval* lval_read(mpc_ast_t* tree);

// Printers
char* lval_str_type(int type);

char* lval_str_expr(lval* node, char open, char close);

char* lval_str_func(lval* func);

char* lval_str(lval* node);

void lval_print(lenv* env, lval* node);

void lval_println(lenv* env, lval* node);

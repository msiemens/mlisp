#include "utils.h"
#include "parser.h"


static int grammar_initialized = 0;
mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;


mpc_parser_t* parser_get() {
    if (grammar_initialized) {
        return Lispy;
    }

    Number   = mpc_new("number");
    Symbol   = mpc_new("symbol");
    String   = mpc_new("string");
    Comment  = mpc_new("comment");
    Sexpr    = mpc_new("sexpr");
    Qexpr    = mpc_new("qexpr");
    Expr     = mpc_new("expr");
    Lispy    = mpc_new("lispy");

    // Define them with the following Language
    mpc_err_t* err = mpca_lang(MPCA_LANG_DEFAULT, /*__GRAMMAR__*/,
                          Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);

    if (err != NULL) {
        mpc_err_print(err);
        mpc_err_delete(err);
        exit(1);
    }

    grammar_initialized = 1;
    return Lispy;
}

void parser_cleanup() {
    mpc_cleanup(7, Number, Symbol, String, Comment,  Sexpr, Qexpr, Expr, Lispy);
}

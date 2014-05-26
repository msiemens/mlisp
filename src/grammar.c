#include "grammar.h"

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;


mpc_parser_t* parser_get() {
    Number   = mpc_new("number");
    Symbol   = mpc_new("symbol");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("qexpr");
    Expr     = mpc_new("expr");
    Lispy    = mpc_new("lispy");

    // Define them with the following Language
    mpca_lang(MPCA_LANG_DEFAULT, /*__GRAMMAR__*/,
              Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    return Lispy;
}

void parser_cleanup() {
    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
}

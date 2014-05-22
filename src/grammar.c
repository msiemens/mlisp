#include "grammar.h"

mpc_parser_t* Number;
mpc_parser_t* Operator;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;


mpc_parser_t* parser_get() {
    Number   = mpc_new("number");
    Operator = mpc_new("operator");
    Expr     = mpc_new("expr");
    Lispy    = mpc_new("lispy");

    // Define them with the following Language
    mpca_lang(MPCA_LANG_DEFAULT,/*__GRAMMAR__*/,
    Number, Operator, Expr, Lispy);

    return Lispy;
}

void parser_cleanup() {
    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    // New comment
}

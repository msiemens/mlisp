#include "mpc.h"

#include "config.h"
#include "lval.h"

lval* eval_sexpr(lval* node);

lval* eval(lval* node);

lval* eval_builtin_op(lval* node, char op);

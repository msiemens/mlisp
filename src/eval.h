#include "mpc.h"

#include "config.h"

#define isint(f) (f - (int) f == 0)

PRECISION eval(mpc_ast_t* t);

PRECISION eval_op(char* op, PRECISION x, PRECISION y);

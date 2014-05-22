#include "mpc.h"

#include "config.h"
#include "lval.h"

/*!
 * Evaluate an AST
 *
 * @param   tree    The AST to evaluate
 */
lval eval(mpc_ast_t* t);

/*!
 * Evaluate a maths operation
 *
 * @param   op  the operator to evaluate
 * @param   x   the fist operand
 * @param   y   the second operand
 * @return      the result of the operation
 */
lval eval_op(char* op, lval x, lval y);

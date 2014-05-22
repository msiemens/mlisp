#include "config.h"
#include "eval.h"

/*!
 * Evaluate an AST
 *
 * @param   tree    The AST to evaluate
 */
PRECISION eval(mpc_ast_t* tree) {

    // If tagged as number return it directly, otherwise expression.
    if (strstr(tree->tag, "number")) { return atof(tree->contents); }

    // The operator is always second child.
    char* op = tree->children[1]->contents;

    // We store the third child in `x`
    PRECISION x = eval(tree->children[2]);

    // Iterate the remaining children
    int i = 3;
    PRECISION y = 0;

    if (strstr(tree->children[i]->tag, "expr") == 0) {
        // If the current expr is consumed, the next tag is "expr" again
        return eval_op(op, x, 0);
    }

    while (strstr(tree->children[i]->tag, "expr")) {
        // While current expr is not consumed
        y = eval(tree->children[i]); // Evaluate next child
        x = eval_op(op, x, y);  // Calculate the result and use it as x
        i++;
    }

    return x;
}

/*!
 * Evaluate a maths operation
 *
 * @param   op  the operator to evaluate
 * @param   x   the fist operand
 * @param   y   the second operand
 * @return      the result of the operation
 */
PRECISION eval_op(char* op, PRECISION x, PRECISION y) {
    if (strcmp(op, "+") == 0) { return x + y; }
    if (strcmp(op, "-") == 0) { return (y == 0) ? -x : x - y; }
    if (strcmp(op, "*") == 0) { return x * y; }
    if (strcmp(op, "/") == 0) { return x / y; }
    if (strcmp(op, "^") == 0) { return pow(x, y); }
    if (strcmp(op, "min") == 0) { return (x < y) ? x : y; }
    if (strcmp(op, "max") == 0) { return (x > y) ? x : y; }
    if (strcmp(op, "%") == 0) {
        if (isint(x) && isint(y)) {
            return (PRECISION_INT) x % (PRECISION_INT) y;
        } else {
            printf("ERROR: Cannot run %% on non-integers\n");
            return 0;
        }
    }
    return 0;
}

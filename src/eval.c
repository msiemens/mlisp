#include <stdbool.h>

#include "eval.h"

static inline bool is_int(float f) { return (f - (int) f == 0); }

lval eval(mpc_ast_t* tree) {

    // If tagged as number return it directly, otherwise expression.
    if (strstr(tree->tag, "number")) {
        // Verify it's a number
        PRECISION val = strtod(tree->contents, NULL);

        if (errno != ERANGE) {
            return lval_num(val);
        } else {
            return lval_err(LERR_BAD_NUM);
        }
    }

    // The operator is always second child.
    char* op = tree->children[1]->contents;

    // We store the third child in `x`
    lval x = eval(tree->children[2]);

    // Iterate the remaining children
    int i = 3;

    if (strstr(tree->children[i]->tag, "expr") == 0) {
        // If the current expr is consumed, the next tag is "expr" again
        return eval_op(op, x, lval_num(0));
    }

    while (strstr(tree->children[i]->tag, "expr")) {
        // While current expr is not consumed
        lval y = eval(tree->children[i]); // Evaluate next child
        x = eval_op(op, x, y);  // Calculate the result and use it as x
        i++;
    }

    return x;
}

lval eval_op(char* op, lval x, lval y) {
    // If either value is an error, return it
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    // Do math
    if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
    if (strcmp(op, "-") == 0) { return lval_num((y.num == 0) ? -x.num : x.num - y.num); }
    if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
    if (strcmp(op, "/") == 0) {
        if (y.num == 0) {
            return lval_err(LERR_DIV_ZERO);
        } else {
            return lval_num(x.num / y.num);
        }
    }
    if (strcmp(op, "^") == 0) { return lval_num(pow(x.num, y.num)); }
    if (strcmp(op, "min") == 0) { return lval_num((x.num < y.num) ? x.num : y.num); }
    if (strcmp(op, "max") == 0) { return lval_num((x.num > y.num) ? x.num : y.num); }
    if (strcmp(op, "%") == 0) {
        if (y.num == 0) {
            return lval_err(LERR_DIV_ZERO);
        }

        if (is_int(x.num) && is_int(y.num)) {
            return lval_num((PRECISION_INT) x.num % (PRECISION_INT) y.num);
        } else {
            return lval_err(LERR_NOT_INT);
        }
    }
    return lval_err(LERR_BAD_OP);
}

#include "lval.h"
#include "parser.h"
#include "eval.h"
#include "builtin.h"

lval* builtin_load(lenv* env, lval* node) {
    LASSERT_ARG_COUNT("load", node, 1);
    LASSERT_ARG_TYPE("load", node, 0, LVAL_STR);

    mpc_result_t r;
    char* filename = node->values[0]->str;

    // TODO: Move to parse.c
    if (mpc_parse_contents(filename, parser_get(), &r)) {
        // Read contents
        lval* expr = parse_tree(r.output);
        mpc_ast_delete(r.output);

        // Evaluate each expression
        while (expr->count) {
            lval* result = eval(env, lval_pop(expr, 0));

            // Handle errors
            if (result->type == LVAL_ERR) {
                lval_println(env, result);
            }
            lval_del(result);
        }

        lval_del(expr); lval_del(node);
        return lval_sexpr();
    } else {
        char* error_message = mpc_err_string(r.error);
        mpc_err_delete(r.error);
        // Remove trailing \n
        error_message[strlen(error_message) - 1] = ' ';
        lval* err;

        if (strstr(error_message, "open file") != 0) {
            err = lval_err("Unable to open file: %s", filename);
        } else {
            err = lval_err("Error loading file%s", error_message);
        }

        xfree(error_message);

        lval_del(node);
        return err;
    }
}

lval* builtin_display(lenv* env, lval* node, bool newline) {
    for_item(node, {
        char* str = lval_to_str(env, item);
        printf("%s", str);
        xfree(str);

        if (i != node->count - 1) {
            putchar(' ');
        }
    });

    if (newline) {
        putchar('\n');
    }

    lval_del(node);
    return lval_sexpr();
}

lval* builtin_println(lenv* env, lval* node) { return builtin_display(env, node, 1); }
lval* builtin_print  (lenv* env, lval* node) { return builtin_display(env, node, 0); }

lval* builtin_repr(lenv* env, lval* node) {
    LASSERT_ARG_COUNT("repr", node, 1);

    char* repr = lval_repr(env, lval_pop(node, 0));

    lval_del(node);
    return lval_str(repr);
}

lval* builtin_error(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("error", node, 1);
    LASSERT_ARG_TYPE("error", node, 0, LVAL_STR);

    lval* err = lval_err(node->values[0]->str);

    lval_del(node);
    return err;

}

lval* builtin_eval(lenv* env, lval* node) {
    LASSERT_ARG_COUNT("eval", node, 1);
    LASSERT_ARG_TYPE("eval", node, 0, LVAL_QEXPR);

    lval* qexpr = lval_take(node, 0);
    qexpr->type = LVAL_SEXPR;

    return eval(env, qexpr);
}

#if defined DEBUG
    lval* builtin_debug_stats(lenv* env, lval* node) {
        UNUSED(env);
        lval_print_stats();

        lval_del(node);
        return lval_sexpr();
    }
#endif

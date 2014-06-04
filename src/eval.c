#include <stdbool.h>
#include <math.h>

#include "lenv.h"
#include "eval.h"


lval* eval_sexpr(lenv* env, lval* node) {
    // Evaluate children
    for (int i = 0; i < node->count; i++) {
        node->values[i] = eval(env, node->values[i]);

        // Error checking
        if (node->values[i]->type == LVAL_ERR) {
            return lval_take(node, i);
        }
    }

    // Empty expression
    if (node->count == 0) { return node; }

    // Single expression
    else if (node->count == 1) { return lval_take(node, 0); }

    // Ensure first element is a symbol
    lval* func = lval_pop(node, 0);
    if (func->type != LVAL_FUNC) {
        char* repr = lval_to_str(env, func);
        lval* error = lval_err("First element is not a function: %s", repr);
        xfree(repr);
        lval_del(func); lval_del(node);
        return error;
    }

    // Call builtin with operator
    lval* result = eval_func(env, func, node);
    lval_del(func);

    return result;
}

// TODO: Improvie this code!
lval* eval_func(lenv* env, lval* func, lval* args) {
    if (func->builtin) {
        return func->builtin(env, args);
    }

    lval* formals = func->formals;
    // Record argument count
    int given = args->count;
    int total = formals->count;

    while (args->count) {
        // If we ran out of formal arguments to bind
        if (formals->count == 0) {
            lval_del(args);
            char* repr = lval_to_str(env, func);
            lval* err = lval_err("Function '%s' passed too many arguments. Expected %i, got %i.",
                                 repr, given, total);
            xfree(repr);
            return err;
        }

        lval* symbol = lval_pop(formals, 0);

        // Handle varargs
        if (strcmp(symbol->sym, "...") == 0) {
            // Ensure one symbols follows
            if (formals->count != 1) {
                lval_del(symbol); lval_del(args);
                return lval_err("Function format is invalid: '...' not followed by single symbol.");
            }

            lval* next_sym = lval_pop(formals, 0);

            lenv_put(func->env, next_sym, builtin_list(env, args));
            lval_del(symbol); lval_del(next_sym);
            break;
        }

        lval* value = lval_pop(args, 0);

        lenv_put(func->env, symbol, value);

        lval_del(symbol);
        lval_del(value);
    }

    lval_del(args);

    // If '...' has not yet been processed, it should be bound to empty list
    if (formals->count > 0 && strcmp(formals->values[0]->sym, "...") == 0) {
        // Ensure that '...' is not passed invalidly
        if (formals->count != 2) {
            return lval_err("Function format invalid: '...' not followed by single symbol.");
        }

        // Delete '...'
        lval_del(lval_pop(formals, 0));

        lval* symbol = lval_pop(formals, 0);
        lval* value  = lval_qexpr();

        lenv_put(func->env, symbol, value);
        lval_del(symbol); lval_del(value);
    }

    // If all formals have been bound, evaluate
    if (formals->count == 0) {
        func->env->parent = env;
        return builtin_eval(func->env, lval_add(lval_sexpr(), lval_copy(func->body)));
    } else {
        return lval_copy(func);
    }
}

lval* eval(lenv* env, lval* node) {
    // Evaluate S-Expressions
    if (node->type == LVAL_SYM) {
        lval* value = lenv_get(env, node);
        lval_del(node);
        return value;
    } else if (node->type == LVAL_SEXPR) {
        return eval_sexpr(env, node);
    }

    return node;
}

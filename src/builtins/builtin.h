/**
* \file    builtin.h
* \brief   Contains all builtin functions.
*/
#pragma once

#if !defined(MLISP_NOINCLUDE)
    #include "stdbool.h"
#endif

#include "parser.h"
#include "lval.h"
#include "eval.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

/**
 * Return an error object.
 *
 * Replaces the given node with an error object and returns it.
 */
#define LERROR(node, ...) \
    lval* error = lval_err(__VA_ARGS__); \
    lval_del(node); \
    return error;\

/**
 * Check an assertion, return an error if it fails.
 */
#define LASSERT(node, cond, ...) \
    if (!(cond)) { \
        LERROR(node, __VA_ARGS__); \
    } \

/**
 * Assert that `ecount` arguments has been passed.
 */
#define LASSERT_ARG_COUNT(name, node, ecount) \
    if (node->count > ecount) { \
        LERROR(node, "Function '%s' passed too many arguments. Expected %i, got %i.", \
               name, ecount, node->count); \
    } else if (node->count < ecount) { \
        LERROR(node, "Function '%s' passed too few arguments. Expected %i, got %i.", \
               name, ecount, node->count); \
    }

/**
* Assert that at least `ecount` arguments has been passed.
*/
#define LASSERT_MIN_ARG_COUNT(name, node, ecount) \
    if (node->count < ecount) { \
        LERROR(node, "Function '%s' passed too few arguments. Expected at least %i, got %i.", \
               name, ecount, node->count); \
    }

/**
* Assert that at most `ecount` arguments has been passed.
*/
#define LASSERT_MAX_ARG_COUNT(name, node, ecount) \
    if (node->count > ecount) { \
        LERROR(node, "Function '%s' passed too many arguments. Expected at most %i, got %i.", \
               name, ecount, node->count); \
    }

/**
* Assert that the `i`-th argument has the type `etype`.
*/
#define LASSERT_ARG_TYPE(name, node, i, etype) \
    LASSERT(node, node->values[i]->type == etype, \
            "Function '%s' passed incorrect argument types. Expected %s, got %s.", \
            name, lval_str_type(etype), lval_str_type(node->values[i]->type));

/**
* Assert that the `i`-th argument is not an empty list.
*/
#define LASSERT_ARG_NOT_EMPTY_LIST(name, node, i) \
    LASSERT(node, node->values[i]->count > 0, \
            "Function '%s' passed empty list.", name);

#pragma GCC diagnostic pop


/// Possible definition scopes.
typedef enum def_scope {DEF_LOCAL, DEF_GLOBAL} def_scope;

/**
 * Add all builtin functions to the environment.
 *
 * \param env   The environment to initialize.
 */
void builtins_init(lenv* env);

/**
 * Create a builtin function in an environment.
 *  
 * Note that any builtin has to transform the given arguments completely
 * or return it. Otherwise memory leaking will occur!
 *
 * \param env   The environment where to add the newly created builtin.
 * \param func  The C function to call.
 * \param name  The name of the function.
 */
void builtin_create(lenv* env, lbuiltin func, char* name);


/**
 * Load and execute an external mlisp file.
 * 
 * The loaded file will be executed in the current environment.
 *
 * \param env   The environment where to run this function.
 * \param node  The filename.
 *
 * \returns None on success or an error.
 */
lval* builtin_load(lenv* env, lval* node);

/**
 * Print an object followed by a newline.
 *
 * \param env   The environment where to run this function.
 * \param node  The object to print.
 *
 * \returns None
 */
lval* builtin_println(lenv* env, lval* node);

/**
 * Print an object.
 *
 * \param env   The environment where to run this function.
 * \param node  The the object to print.
 *
 * \returns None
 */
lval* builtin_print(lenv* env, lval* node);

/**
 * Return the string representation of an object.
 *
 * \param env   The environment where to run this function.
 * \param node  The object to get the repr of.
 *
 * \returns The string object.
 */
lval* builtin_repr(lenv* env, lval* node);

/**
 * Return an error with a given error message.
 *
 * \param env   The environment where to run this function.
 * \param node  The error message.
 *
 * \returns The string object.
 */
lval* builtin_error(lenv* env, lval* node);

/**
 * Evaluate a list as an expression
 *
 * \param env   The environment where to run this function.
 * \param node  The list to evaluate.
 *
 * \returns The result of the evaluation.
 */
lval* builtin_eval(lenv* env, lval* node);

// -------------------- DOC MARKER --------------------

/**
* SHORT_DESCR
*
* \param env   The environment where to run this function.
* \param node  The arguments.
*
* \returns RETURN_VALUE
*/
lval* builtin_add(lenv* env, lval* node);


lval* builtin_sub(lenv* env, lval* node);

lval* builtin_mul(lenv* env, lval* node);

lval* builtin_div(lenv* env, lval* node);

lval* builtin_mod(lenv* env, lval* node);

lval* builtin_op(lenv* env, lval* node, char op);


lval* builtin_gt(lenv* env, lval* node);

lval* builtin_ge(lenv* env, lval* node);

lval* builtin_lt(lenv* env, lval* node);

lval* builtin_le(lenv* env, lval* node);

lval* builtin_ord(lenv* env, lval* node, char* op);

lval* builtin_eq(lenv* env, lval* node);

lval* builtin_ne(lenv* env, lval* node);

lval* builtin_cmp(lenv* env, lval* node, char* op);

lval* builtin_and(lenv* env, lval* node);

lval* builtin_or(lenv* env, lval* node);

lval* builtin_not(lenv* env, lval* node);

lval* builtin_if(lenv* env, lval* node);


lval* builtin_head(lenv* env, lval* node);

lval* builtin_tail(lenv* env, lval* node);

lval* builtin_list(lenv* env, lval* node);

lval* builtin_join(lenv* env, lval* node);

lval* builtin_cons(lenv* env, lval* node);


lval* builtin_def(lenv* env, lval* node);

lval* builtin_put(lenv* env, lval* node);

lval* builtin_var(lenv* env, lval* node, def_scope type);

lval* builtin_lambda(lenv* env, lval* node);


#if defined DEBUG
    lval* builtin_debug_stats(lenv* env, lval* node);
#endif

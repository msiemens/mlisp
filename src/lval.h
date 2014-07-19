/**
 * \file    lval.h
 * \brief   Contains the lval object and it's functions.
 */
#pragma once

#include <stdbool.h>

#if !defined(MLISP_NOINCLUDE)
    #include "mpc.h"
#endif

#include "config.h"
#include "utils.h"


// Forward declarations
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

/// Pointer to a builtin function. Has to take a #lenv and #lval pointer and
/// return a #lval pointer.
typedef lval* (*lbuiltin) (lenv*, lval*);

/// Possible #lval object types.
typedef enum lval_type {
    LVAL_SEXPR, ///< A S-Expression.
    LVAL_QEXPR, ///< A Q-Expression.
    LVAL_SYM,   ///< A symbol.
    LVAL_FUNC,  ///< A function (lambda or builtin).
    LVAL_NUM,   ///< A floating point number.
    LVAL_STR,   ///< A string.
    LVAL_ERR    ///< An error.
} lval_type;

/// The lval object.
struct lval {
    lval_type type; ///< The object's type

    // Data
    union {
        PRECISION_FLOAT num;    ///< Value of a number object.
        char* err;              ///< Value of an error object.
        char* sym;              ///< Value of a symbol object.
        char* str;              ///< Value of a string object.

        /// Values for S-Expr/Q-Expr
        struct {
            int count;              ///< The number of values.
            struct lval** values;   ///< The values (pointer to pointers).
        };

        /// Value of function object
        struct {
            lbuiltin builtin;   ///< Pointer to a builtin function or ...
            lenv* env;          ///< a lambda function with an environment,
            lval* formals;      ///< formal arguments and
            lval* body;         ///< a function body.
        };
    };
};


/// The size of a lval object
static const size_t LVAL_SIZE = sizeof(lval);

/// The size of a pointer to a lval object
static const size_t LVAL_PTR_SIZE = sizeof(lval*);


#if !defined(MLISP_NOINCLUDE)
    // Forward declarations
    lenv* lenv_new(void);
    void lenv_del(lenv* env);
    lenv* lenv_copy(lenv* env);
    char* lenv_func_name(lenv* env, lbuiltin func);
#endif


// ------------------------------------------------------------------------------
// Constructors & destructor

/**
 * Allocate memory for a new #lval object.
 * 
 * Allocate memory to contain a new #lval object. This space won't be initialized
 * and thus may contain random data. This function is guaranteed to return a
 * valid pointer.
 *
 * \returns A pointer to the new object.
 */
lval* lval_new(void);

/**
 * Create and initialize a lispy S-Expression.
 * 
 * \returns A pointer to the newly created object.
 */
lval* lval_sexpr(void);

/**
 * Create and initialize a lispy Q-Expression.
 *
 * \returns A pointer to the newly created object.
 */
lval* lval_qexpr(void);

/**
 * Create and initialize a lispy symbol.
 *
 * \param symbol    The symbol's value. Won't be deallocated,
 *                  you'll have to cleanup yourself.
 * \returns A pointer to the newly created object.
 */
lval* lval_sym(char* symbol);

/**
 * Create and initialize a lispy number.
 *
 * \param value     The number's value.
 * \returns A pointer to the newly created object.
 */
lval* lval_num(PRECISION_FLOAT value);

/**
 * Create and initialize a lispy string.
 *
 * \param str   The string's value. Won't be deallocated,
 *              you'll have to cleanup yourself.
 * \returns A pointer to the newly created object.
 */
lval* lval_str(char* str);

/**
 * Create and initialize a lispy error.
 * 
 * Recieves additional arguments to act `printf`-like.
 *
 * \param fmt   The error message format string. Won't be deallocated,
 *                  you'll have to cleanup yourself.
 * \param ...   Format arguments.
 * \returns A pointer to the newly created object.
 */
lval* lval_err(char* fmt, ...);

/**
 * Create and initialize a builtin lispy function.
 *
 * \param [in] func     The function to run when calling the function.
 * \returns A pointer to the newly created object.
 */
lval* lval_func(lbuiltin func);

/**
 * Create and initialize a lambda function.
 * 
 * Defines a lambda function. It takes a q-expr containing the formal
 * arguments and a q-expr with the function body.
 *
 * \todo     Rename `formals` to `arguments`.
 *
 * \param formals   The formal arguments.
 * \param body      The function body.
 *
 * \returns A pointer to the newly created object.
 */
lval* lval_lambda(lval* formals, lval* body);

/**
 * Delete a #lval object.
 * 
 * Delete a #lval object. If the object holds a string, it frees it.
 * 
 * \li If it's a lambda, it first deletes the env, formals and body.
 * \li If it's a qexpr or sexpr, it first deletes all child objects.
 *
 * Then finally, it frees it's own memory.
 *
 * \param node  The object to delete.
 */
void lval_del(lval* node);

/**
 * Copy a #lval object.
 * 
 * Copy a #lval object. The new object is completely independent from
 * the original one, it holds no references to any string or child object
 * of the original.
 *
 * \param node  The object to copy.
 * \returns A pointer to the copied object.
 */
lval* lval_copy(lval* node);

/**
 * Check for equality of two objects's values.
 * 
 * Compare two object's values. If the two objects have different types,
 * the comparison always is false. Otherwise, the values are compared.
 *
 * \param x The first object
 * \param y The second object
 *
 * \returns         The result of the comparison.
 * \retval false    The values are different.
 * \retval true     The values are equal.
 */
bool lval_eq(lval* x, lval* y);


// ------------------------------------------------------------------------------
// Lvalue modifiers


/**
 * Add an object to a container.
 *
 * On a list-like container (Q-Expr or S-Expr), add the object given by
 * value to the end of the container values list. Has to be performed in-place:
 * \code
 *      container = lval_add(container, value);
 * \endcode
 *
 * \param container [out]   The container where to add the value.
 * \param value             The value to add.
 *
 * \returns The new container with the value added.
 */
lval* lval_add(lval* container, lval* value);

/**
 * Join two lists into a new one.
 *
 * Takes two lists and joins them into a new one and returns it. Deletes both
 * arguments and creates a new object. Has to be performed in-place:
 * \code
 *      container = lval_join(fist, second);
 * \endcode
 *
 * \param first     The first list.
 * \param second    The second list.
 *
 * \returns The new container with the values from both lists.
 */
lval* lval_join(lval* first, lval* second);

/**
 * Pop a value from a list and returns it.
 *
 * Remove a value from a list-like container and return it. The container
 * now doesn't contain the returned value any more.
 *
 * \warning Please note that from now on you have to memory-manage the return
 *          value, too!
 *
 * \param container [out]   The list from which to pop the element.
 * \param index             Which element to pop (zero-based).
 *
 * \returns The value at `container[index]`.
 */
lval* lval_pop(lval* container, int index);

/**
 * Return an element from a list and remove the rest of the list.
 *
 * Pop an element from a list-like container, return it and remove the rest
 * of the container. Essentially like running
 * \code
 *      lval* item = lval_pop(container, index);
 *      lval_del(container);
 * \endcode
 *
 * \param container [out]   The list from which to pop the element.
 * \param index             Which element to pop (zero-based).
 *
 * \returns The value at `container[index]`.
 */
lval* lval_take(lval* container, int index);


// ------------------------------------------------------------------------------
// Printers

/**
 * Return the string representation of a lval type.
 */
char* lval_str_type(lval_type type);

/**
 * Return the string representation of a Q-Expr/S-Expr.
 *
 * Build and return the string representation of a Q-Expr/S-Expr.
 *
 * \param env   The environment from where the print is called.
 * \param node  The expression object to stringify.
 * \param open  The opening character: `(` or `{`.
 * \param close The closing character: `)` or `}`.
 *
 * \returns A char pointer containing the string. Has to be cleaned up!
 */
char* lval_str_expr(lenv* env, lval* node, char open, char close);

/**
 * Return the string representation of a function.
 *
 * Build and return the string representation of a function. If it's a builtin
 * function, the function returns `<function name>`.
 * If it's a lambda function, the function returns it's definition.
 *
 * \todo If lambda: Replace formals that are set with their values.
 * 
 * \param env   The environment from where the print is called.
 * \param func  The function object to stringify.
 *
 * \returns A char pointer containing the string. Has to be cleaned up!
 */
char* lval_str_func(lenv* env, lval* func);

/**
 * Return the string representation of a string.
 *
 * Build and return the string representation of a string object.
 *
 * \param env   The environment from where the print is called.
 * \param node  The string object to stringify.
 *
 * \returns A char pointer containing the string. Has to be cleaned up!
 */
char* lval_str_str(lenv* env, lval* node);

/**
 * Convert a value to a string.
 *
 * \param env   The environment from where the print is called.
 * \param node  The object to stringify.
 *
 * \returns A char pointer containing the string. Has to be cleaned up!
 */
char* lval_to_str(lenv* env, lval* node);

/**
 * Take a value and return the string representation of it.
 *
 * Differs from #lval_to_str by escaping all quotes and backslashed when
 * getting a string.
 *
 * \param env   The environment from where the print is called.
 * \param node  The object to represent.
 *
 * \returns A char pointer containing the string. Has to be cleaned up!
 */
char* lval_repr(lenv* env, lval* node);

/**
 * Print a #lval object.
 *
 * \param env   The environment from where the print is called.
 * \param node  The object to print.
 */
void lval_print(lenv* env, lval* node);

/**
* Print a #lval object and a newline.
*
* \see lval_print
*/
void lval_println(lenv* env, lval* node);

#if defined(DEBUG)
    /**
     * Print some internal statistics.
     */
    void lval_print_stats(void);
#endif


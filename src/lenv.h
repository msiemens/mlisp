/**
* \file    lenv.h
* \brief   Contains the lenv object and it's functions.
*/
#pragma once

#if defined(MLISP_NOINCLUDE)
    typedef struct {
        uint32_t n_buckets, size, n_occupied, upper_bound;
        uint32_t* flags;
        const char* *keys;
        void* *vals;
    } hash_t;
#else
    #include "hash.h"
#endif

#include "utils.h"
#include "lval.h"


/// Contains an environment.
typedef struct lenv {
    lenv* parent;   ///< The parent enrivonment.
    hash_t* values; ///< The values in this environment.
} lenv;


// ------------------------------------------------------------------------------
// Constructor & destructor

/**
* Allocate memory for a new #lenv object.
*
* Allocate memory to contain a new #lenv object. This space won't be initialized
* and thus may contain random data. This function is guaranteed to return a
* valid pointer.
*
* \returns A pointer to the new envirnment.
*/
lenv* lenv_new(void);

/**
 * Delete an environment.
 *
 * Delete an environment by first removing all values stored in it and then
 * the environment itself.
 *
 * \param env   The environment to remove.
 */
void lenv_del(lenv* env);

/**
 * Copy an environment.
 *
 * Copy an environment by creating a new one and copying all the values.
 * The new environment will be a child of the original one.
 *
 * \todo Rename to something more intuitive (create a child...)
 *
 * \param env   The environment to copy.
 */
lenv* lenv_copy(lenv* env);

// ------------------------------------------------------------------------------
// Getters & setters

/**
 * Search the environment for a variable with a given name.
 *
 * Get a value from an environment. If it is not found, search in the parent
 * environment. If this fails, return a #lval_err.
 *
 * \warning Returns a copy of the value that has to be memory-managed, too!
 *
 * \param env   The envionment where to start searching.
 * \param name  The name of the variable to search.
 *
 * \returns A copy of the value stored in the environment.
 */
lval* lenv_get(lenv* env, lval* name);

/**
 * Store a value in an environment.
 *
 * Store the copy of the given value in given environment.
 *
 * \param env   The environment where to store the value.
 * \param name  The name of the variable.
 * \param value The value to store.
 */
void lenv_put(lenv* env, lval* name, lval* value);

/**
 * Store a value in the outermost environment.
 *
 * \see lenv_put
 *
 * \param env   The environment where to seek the outermost environment
 *              where to store the value.
 * \param name  The name of the variable.
 * \param value The value to store.
 */
void lenv_def(lenv* env, lval* name, lval* value);

/**
 * Get the name of a builtin function.
 *
 * Search the environment for a builtin function to get it's name.
 *
 * \param env   Where to search for the function.
 * \param func  The function to search.
 *
 * \returns A copy of the function name. Has to be cleaned up!
 * \retval  NULL    The function is not defined in this environment.
 */
char* lenv_func_name(lenv* env, lbuiltin func);

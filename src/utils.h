/**
 * \file    utils.h
 * \brief   Several utils used everywhere.
 */
#pragma once

#if !defined(MLISP_NOINCLUDE)
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
#endif


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

/// Tell GNU-compilers to allow non-ANSI extensions. Otherwise: no effect.
#if !defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    #define __extension__(arg) arg
#endif

#if ! defined NDEBUG
    /// DEBUG flag
    #define DEBUG 1
#endif

/// Prevent compiler from complaining about unused arguments
#define UNUSED(x) (void)(x)


// ------------------------------------------------------------------------------
// Assertions with custom messages

/**
 * Log an error message.
 *
 * Print an error message to `stderr`.
 *
 * \param msg   The message to print.
 * \param ...   Additional `printf`-like arguments.
 */
#define LOG_ERROR(msg, ...) __extension__({ \
    fprintf(stderr, "[ERROR] (%s:%d): " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
})

#if defined DEBUG
    /**
     * Assert with custom message.
     *
     * Make an assertion. If it fails, print an error to `stderr`.
     *
     * \param ...   First argument: the message to print. Then: Additional
     *                  `printf`-like arguments.
     */
    #define ASSERTF(cond, ...) __extension__({ \
        if(!(cond)) { \
            LOG_ERROR(__VA_ARGS__); \
            assert(cond); \
        } \
    })

    /**
     * Make assertions about a function argument.
     *
     * Make an assertion about a function argument. If it fails, print
     * a helpful error message to `stderr`.
     *
     * Prints an error like:
     * \verbatim [ERROR] (file:line): 'char* strdup(const char *)':s is a NULL pointer \endverbatim
     *
     * \param name  The name of the argument to check.
     * \param test  The test to execute.
     * \param msg   An additional error message.
     * \param ...   Additional `printf`-like arguments.
     */
    #define ASSERT_ARG(name, test, msg, ...) __extension__({\
        ASSERTF(test, "'%s':%s " msg, __PRETTY_FUNCTION__, #name, __VA_ARGS__);\
    })

    /**
     * Assert that an argument is a list-like #lval.
     *
     * Assert that an argument is a list-like #lval (#LVAL_SEXPR or #LVAL_QEXPR).
     *
     * \param name  The name of the argument to check.
     */
    #define ASSERT_LIST_LIKE(name)\
        ASSERT_ARG(name, is_list_like(name),\
                   "is of type %s which is not list-like a container",\
                   lval_str_type(name->type));

    /**
     * Assert that an argument is not the NULL pointer.
     *
     * \param name  The name of the argument to check.
     */
    #define ASSERT_NOT_NULL(name)\
        ASSERT_ARG(name, name != NULL,\
                   "is a NULL pointer%s", "");
#else
    #define ASSERTF(...)
    #define ASSERT_ARG(...)
    #define ASSERT_LIST_LIKE(...)
    #define ASSERT_NOT_NULL(...)
#endif

#pragma GCC diagnostic pop


/**
 * Duplicate a string.
 *
 * Return a pointer to a new string with is a duplicate of the given string.
 * Has to be cleaned up!
 *
 * \param s The string to duplicate.
 *
 * \returns A pointer to the new string.
 */
char* strdup(const char * s);

/**
 * A safe sprintf.
 *
 * Like sprintf but allocates a buffer safely by checking the required size
 * first.
 *
 * \param fmt	The format string.
 * \param ...   `printf`-like arguments.
 *
 * \returns A string.
 */
 char* xsprintf(const char* fmt, ...);

/**
 * Append a string to another string.
 *
 * Copy a string to the end of another string. Needs the new total length.
 *
 * \param [in,out]  dest	Where to append the string to.
 * \param [in]      src	The string to append.
 * \param           size	The size of the new string.
 *
 * \returns A char pointer to `dest` (**might be moved!**)
 */
char* strappend(char* dest, char* src, size_t size);

/**
 * Free a pointer safely
 *
 * Free a pointer while checking for double free and preventing use after free
 * by setting the pointer to NULL.
 *
 * \param ptr	The pointer to free.
 */
void xfree(void* ptr);

/**
 * Allocate memory safely.
 *
 * Allocate memory safely by checking for a failed `malloc`. If it fails,
 * the program dies with an error message.
 *
 * \param n	    The size of the memory to allocate.
 *
 * \returns A pointer to the allocated memory.
 */
 void* xmalloc(size_t n);

/**
 * Reallocate a pointer safely.
 *
 * Reallocate a pointer safely by checking for a failed `realloc`. If it fails,
 * the program dies with an error message.
 *
 * \param ptr	The pointer to reallocate.
 * \param n	    The new size of the memory block.
 *
 * \returns A pointer to the reallocated memory.
 */
 void* xrealloc(void* ptr, size_t n);

/**
 * Abort the program with an error message.
 *
 * \param msg	The message to display before aborting.
 */
void die(char* msg);

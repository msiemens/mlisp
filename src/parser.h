/**
 * \file    parser.h
 * \brief   Parsing methods.
 */

#pragma once

#if !defined(MLISP_NOINCLUDE)
    #include "mpc.h"
#endif

#include <stdbool.h>

#include "lval.h"

/**
 * Initialize the parser.
 *  
 * Has to be called before #parser_get is ever invoked!
 */

void parser_init(void);

/**
 * Get the parser object.
 *
 * \returns A pointer to the parser object.
 */
mpc_parser_t* parser_get(void);

/**
 * Delete the parser object.
 */
void parser_cleanup(void);

/// Call mpc_err_delete on error!
bool parse(char* filename, char* str, lenv* env, lval** result, mpc_err_t** parser_error);

lval* parse_tree(mpc_ast_t* tree);

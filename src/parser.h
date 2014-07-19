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
#include "eval.h"


/**
 * Get the parser object.
 * 
 * \returns A pointer to the parser object.
 */
mpc_parser_t* parser_get();

/**
 * Delete the parser object.
 */
void parser_cleanup();

bool parse(char* filename, char* str, lenv* env, lval** result, mpc_err_t* parser_error);

lval* parse_tree(mpc_ast_t* tree);

lval* parse_num(mpc_ast_t* tree);

lval* parse_str(char* contents);

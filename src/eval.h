/*!
 * \file    eval.h
 * \brief   Methods to evaluate #lval objects.
 */
#pragma once

#if ! defined MLISP_NOINCLUDE
    #include "mpc.h"
#endif

#include "lval.h"
#include "lenv.h"


/**
 * Evaluate a node.
 *
 * If it's a symbol, return it's value from the given environment. If it's
 * a S-Expression, evaluate it according to #eval_sexpr. Otherwise, just return
 * it.
 *
 * \param env	The environment in which to evaluate the node.
 * \param node	The node to evaluate.
 *
 * \returns A #lval containing the result.
 */
 lval* eval(lenv* env, lval* node);

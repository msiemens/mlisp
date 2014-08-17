/*!
 * \file    eval.h
 * \brief   Methods to evaluate #lval objects.
 */
#pragma once

#if !defined(MLISP_NOINCLUDE)
    #include "mpc.h"
#endif

#include "config.h"
#include "lval.h"
#include "lenv.h"
#include "builtins/builtin.h"


 /**
  * Evaluate a S-Expression.
  *
  * Evaluate a S-Expression: `(func arg1 arg2 ...)`. If there are no arguments,
  * `func` will be returned. Otherwise, `func` is called with the given
  * arguments. Consumes node completely.
  *
  * \param env	The environment in which to evaluate the node.
  * \param node	The node to evaluate.
  *
  * \returns A #lval containing the result.
  */
  lval* eval_sexpr(lenv* env, lval* node);

/**
 * Evaluate a function.
 *
 * Evaluate a function. If it's a builtin, call it straight forward. Otherwise
 * it's a user defined lambda. If there aren't enough arguments to call it,
 * return a new function with the given arguments bound to the lambda context
 * (partial evaluation). Otherwise, evaluate the lambda and return the result.
 *
 * \todo Improve this code
 *
 * \param env	The environment in which to evaluate the function.
 * \param func	The function to call.
 * \param args	A list of arguments for the function.
 *
 * \returns A #lval containing the result.
 */
 lval* eval_func(lenv* env, lval* func, lval* args);

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

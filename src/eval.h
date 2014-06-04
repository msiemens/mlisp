#pragma once

#if !defined(MLISP_NOINCLUDE)
    #include "mpc.h"
#endif

#include "config.h"
#include "lval.h"
#include "lenv.h"
#include "builtins/builtin.h"


lval* eval_sexpr(lenv* env, lval* node);

lval* eval_func(lenv* env, lval* func, lval* args);

lval* eval(lenv* env, lval* node);

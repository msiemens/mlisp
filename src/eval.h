#pragma once

#include "mpc.h"

#include "config.h"
#include "lval.h"
#include "lenv.h"
#include "builtin.h"

lval* eval_sexpr(lenv* env, lval* node);

lval* eval(lenv* env, lval* node);

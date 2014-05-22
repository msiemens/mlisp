#pragma once

#include "config.h"

//! Value container
typedef struct {
    int type;
    int err;
    PRECISION num;
} lval;

//! Value types
enum { LVAL_NUM, LVAL_ERR };

//! Error types
enum { LERR_DIV_ZERO, LERR_NOT_INT, LERR_BAD_OP, LERR_BAD_NUM };


lval lval_num(PRECISION value);

lval lval_err(int errcode);

void lval_print(lval val);

void lval_println(lval val);

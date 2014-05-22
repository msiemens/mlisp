#include <stdio.h>

#include "lval.h"

lval lval_num(PRECISION value) {
    lval val;
    val.type = LVAL_NUM;
    val.num = value;

    return val;
}

lval lval_err(int errcode) {
    lval val;
    val.type = LVAL_ERR;
    val.err = errcode;

    return val;
}

void lval_print(lval val) {
  switch (val.type) {
    case LVAL_NUM:
      printf("%f", val.num);
      break;

    case LVAL_ERR:
      // Check what exact type of error it is and print it
      if (val.err == LERR_DIV_ZERO) { printf("Error: Division by zero!"); }
      else if (val.err == LERR_BAD_OP)   { printf("Error: Invalid operator!"); }
      else if (val.err == LERR_BAD_NUM)  { printf("Error: Invalid number!"); }
      else if (val.err == LERR_NOT_INT)  { printf("Error: Argument is not an integer!"); }
      else { printf("This should never happen!"); }
      break;
  }
}

void lval_println(lval val) { lval_print(val); putchar('\n'); }

#if defined(MLISP_NOINCLUDE)
    typedef struct mpc_parser_t mpc_parser_t;
    typedef struct mpc_err_t mpc_err_t;
    typedef struct mpc_ast_t mpc_ast_t;
#else
    #include "mpc.h"
#endif

#include "config.h"

#if !defined(MLISP_NOINCLUDE)
    #include "utils.h"
#endif

#include "parser.h"
#include "builtins/builtin.h"
#include "eval.h"
#include "lenv.h"
#include "lval.h"

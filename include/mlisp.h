#if defined(MLISP_NOINCLUDE)
    struct mpc_parser_t;
    typedef struct mpc_parser_t mpc_parser_t;
    typedef struct {
        int pos;
        int row;
        int col;
    } mpc_state_t;
    typedef struct mpc_ast_t {
        char *tag;
        char *contents;
        mpc_state_t state;
        int children_num;
        struct mpc_ast_t** children;
    } mpc_ast_t;

    typedef struct {
        mpc_state_t state;
        int expected_num;
        char *filename;
        char *failure;
        char **expected;
        char recieved;
    } mpc_err_t;
    typedef void mpc_val_t;
    typedef union {
        mpc_err_t *error;
        mpc_val_t *output;
    } mpc_result_t;

    int mpc_parse(const char *filename, const char *string, mpc_parser_t *p, mpc_result_t *r);

    void mpc_ast_delete(mpc_ast_t *a);
    void mpc_ast_print(mpc_ast_t *a);

    void mpc_err_delete(mpc_err_t *e);
    char *mpc_err_string(mpc_err_t *e);
    void mpc_err_print(mpc_err_t *e);

#else
    #include "mpc.h"
#endif

#include "config.h"

#if !defined(MLISP_NOINCLUDE)
    #include "utils.h"
#endif

#include "grammar.h"
#include "builtin.h"
#include "eval.h"
#include "lenv.h"
#include "lval.h"

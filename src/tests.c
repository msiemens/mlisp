#include "ptest.h"
#include "mpc.h"

#include "grammar.h"
#include "lval.h"
#include "lenv.h"
#include "eval.h"
#include "builtin.h"

static int INVALID_CMD = 0;

#define PRINT_IF_FAIL(check) \
    if (!(check)) { \
        puts("Result: "); \
        lval_println(env, result); \
    }
#define ASSERT(cmd, check) {\
    lval* result = parse(cmd); \
    if (result) { \
        PRINT_IF_FAIL(check); \
        PT_ASSERT(check); \
        lval_del(result); \
    } else { \
        puts("Invalid command line: " #cmd "! "); \
        PT_ASSERT(INVALID_CMD); \
    } \
}

#define IS_A(t) (result->type == t)
#define IS_ERR() IS_A(LVAL_ERR)
#define IS_FUNC() IS_A(LVAL_FUNC)
#define IS_NUM() IS_A(LVAL_NUM)
#define IS_SEXPR() IS_A(LVAL_SEXPR)
#define IS_QEXPR() IS_A(LVAL_QEXPR)
#define INT_EQUALS(val) (IS_NUM() && result->num == (PRECISION) val)
#define HAS_COUNT(c) (result->count == c)
#define HAS_MSG(msg) (strcmp(result->err, msg) == 0)

#define VALUE_AT(i) result->values[i]
#define QIS_A(i, t) (VALUE_AT(i)->type == t)
#define QIS_NUM(i) QIS_A(i, LVAL_NUM)
#define QINT_EQUALS(i, val) (QIS_NUM(i) && VALUE_AT(i)->num == (PRECISION) val)

static mpc_parser_t* parser;
static lenv* env;

void cleanup(void) {
    lenv_del(env);
    parser_cleanup();
}

void init(void) {
    parser = parser_get();
    env = lenv_new();
    builtins_init(env);
}

void reset(void) {
    cleanup();
    init();
}

lval* parse(char* line) {
    mpc_result_t r;
    if (mpc_parse("<tests>", line, parser, &r)) {
        lval* result = eval(env, lval_read(r.output));
        mpc_ast_delete(r.output);
        return result;
    } else {
        return NULL;
    }
}

void test_variables(void) {
    // Builtin functions
    ASSERT("*", IS_FUNC());

    // Variables
    // FIXME: return value of lval_str has to bee freed, memory leak otherwise!
    ASSERT("{1 2 3}",     strcmp(lval_str(result), "{1 2 3}") == 0);
    ASSERT("def {x} 100", 1);
    ASSERT("x",           INT_EQUALS(100));

    ASSERT("y",           IS_ERR() && HAS_MSG("Unbound symbol: 'y'"));

    ASSERT("eval (head {+ - + - * /})", IS_FUNC());

    ASSERT("def {a b} 5 6", IS_SEXPR());
    ASSERT("+ a b",         INT_EQUALS(11));

    ASSERT("def {arglist} {a b}", IS_SEXPR());
    ASSERT("def arglist 1 2",     IS_SEXPR());
    ASSERT("list a b",            IS_QEXPR() && HAS_COUNT(2)
                                  && QINT_EQUALS(0, 1) && QINT_EQUALS(1, 2));

    reset();

    // Try redefining builtins
    ASSERT("def {+} {}",    IS_ERR() && HAS_MSG("Cannot redefine builtin '+'."));
    ASSERT("def {-} {}",    IS_ERR() && HAS_MSG("Cannot redefine builtin '-'."));
    ASSERT("def {*} {}",    IS_ERR() && HAS_MSG("Cannot redefine builtin '*'."));
    ASSERT("def {/} {}",    IS_ERR() && HAS_MSG("Cannot redefine builtin '/'."));
    ASSERT("def {%} {}",    IS_ERR() && HAS_MSG("Cannot redefine builtin '%'."));
    ASSERT("def {head} {}", IS_ERR() && HAS_MSG("Cannot redefine builtin 'head'."));
    ASSERT("def {tail} {}", IS_ERR() && HAS_MSG("Cannot redefine builtin 'tail'."));
    ASSERT("def {list} {}", IS_ERR() && HAS_MSG("Cannot redefine builtin 'list'."));
    ASSERT("def {eval} {}", IS_ERR() && HAS_MSG("Cannot redefine builtin 'eval'."));
    ASSERT("def {cons} {}", IS_ERR() && HAS_MSG("Cannot redefine builtin 'cons'."));
    ASSERT("def {def} {}",  IS_ERR() && HAS_MSG("Cannot redefine builtin 'def'."));

    // Qexpr
    ASSERT("{}", IS_QEXPR() && HAS_COUNT(0));
}

void test_builtins(void) {
    // list
    ASSERT("list 1 2 3", IS_QEXPR() && HAS_COUNT(3)
                         && QINT_EQUALS(0, 1) && QINT_EQUALS(1, 2)
                         && QINT_EQUALS(2, 3));

    // head
    ASSERT("head {1 2 3}", IS_QEXPR() && HAS_COUNT(1) && QINT_EQUALS(0, 1));
    // Error cases
    ASSERT("head {1} {1}", IS_ERR() && HAS_MSG("Function 'head' passed too many arguments. Expected 1, got 2."));
    ASSERT("head 1",       IS_ERR() && HAS_MSG("Function 'head' passed incorrect argument types. Expected Q-Expression, got number."));
    ASSERT("head {}",      IS_ERR() && HAS_MSG("Function 'head' passed empty list."));

    // tail
    ASSERT("tail {1 2 3}", IS_QEXPR() && HAS_COUNT(2)
                           && QINT_EQUALS(0, 2) && QINT_EQUALS(1, 3));
    // Error cases
    ASSERT("tail {1} {1}", IS_ERR() && HAS_MSG("Function 'tail' passed too many arguments. Expected 1, got 2."));
    ASSERT("tail 1",       IS_ERR() && HAS_MSG("Function 'tail' passed incorrect argument types. Expected Q-Expression, got number."));
    ASSERT("tail {}",      IS_ERR() && HAS_MSG("Function 'tail' passed empty list."));

    // eval
    ASSERT("eval {head (list 1 2 3 4)}", IS_QEXPR() && HAS_COUNT(1) && QINT_EQUALS(0, 1))
    // Error cases
    ASSERT("eval {1} {2}", IS_ERR() && HAS_MSG("Function 'eval' passed too many arguments. Expected 1, got 2."));
    ASSERT("eval 1",       IS_ERR() && HAS_MSG("Function 'eval' passed incorrect argument types. Expected Q-Expression, got number."));

    // join
    ASSERT("join {1} {2}", IS_QEXPR() && HAS_COUNT(2)
                          && QINT_EQUALS(0, 1) && QINT_EQUALS(1, 2))
    // Error cases
    ASSERT("join {1} 1", IS_ERR() && HAS_MSG("Function 'join' passed incorrect argument types. Expected Q-Expression, got number."));

    // cons
    ASSERT("cons {1} {2}", IS_QEXPR() && HAS_COUNT(2) && QIS_A(0, LVAL_QEXPR) && QIS_A(1, LVAL_NUM));
    ASSERT("cons {1}",     IS_ERR() && HAS_MSG("Function 'cons' passed too few arguments. Expected 2, got 1."));
    ASSERT("cons 1 2",     IS_ERR() && HAS_MSG("Function 'cons' passed incorrect argument types. Expected Q-Expression, got number."));

    // lambda
    ASSERT("lambda {x y} {+ x y}",         IS_FUNC());
    ASSERT("(lambda {x y} {+ x y}) 10 20", INT_EQUALS(30));
    ASSERT("def {add-mul} (lambda {x y} {+ x (* x y)})", IS_SEXPR());
    ASSERT("add-mul 10 20",                INT_EQUALS(210));
    ASSERT("add-mul 10",                   IS_FUNC());
    ASSERT("(add-mul 10) 50",              INT_EQUALS(510));
    ASSERT("(lambda {x ... a} {a})",       IS_FUNC());
    ASSERT("(lambda {x ... a} {a})    1",  IS_QEXPR() && HAS_COUNT(0));
    ASSERT("(lambda {x ... a} {a})   1 2", IS_QEXPR() && HAS_COUNT(1) && QINT_EQUALS(0, 2));
    ASSERT("(lambda {x eval} {})     1 2", IS_ERR());
    ASSERT("(lambda {x ... eval} {}) 1 2", IS_ERR());
    reset();
}

void test_maths(void) {
    // Interger math
    ASSERT("+ 5 6",                 INT_EQUALS(11));
    ASSERT("- (* 10 10) (+ 1 1 1)", INT_EQUALS(97));
    ASSERT("- (/ 10 2) 20",         INT_EQUALS(-15));
    ASSERT("- 1",                   INT_EQUALS(-1));
    ASSERT("% 17 12",               INT_EQUALS(5));

    // Float math
    ASSERT("/ 1 2",   INT_EQUALS(0.5));
    ASSERT("+ 1.5 1", INT_EQUALS(2.5));
    ASSERT("- 0.1 10", INT_EQUALS(-9.9));
    ASSERT("* 0.1 10", INT_EQUALS(1));

    // Math errors
    ASSERT("/ 10 0", IS_ERR() && HAS_MSG("Division by zero"));
    ASSERT("% 10 0", IS_ERR() && HAS_MSG("Division by zero"));
    ASSERT("% * ()", IS_ERR() && HAS_MSG("Function '%' passed incorrect argument types. Expected number, got function."));
}

void test_strings(void) {
    PT_ASSERT_STR_EQ("x", "x");
}

void suite_basic(void) {
    pt_add_test(test_maths, "Maths tests", "Suite Basic");
    pt_add_test(test_variables, "Variables", "Suite Basic");
    pt_add_test(test_builtins, "Builtin functions", "Suite Basic");
    pt_add_test(test_strings, "Test Strings", "Suite Basic");
}

int main(int argc, char** argv) {
    init();

    pt_add_suite(suite_basic);
    int status = pt_run();

    cleanup();
    return status;
}
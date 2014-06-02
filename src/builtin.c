#include "builtin.h"

// TODO: Too easy to forget to update, make dynamic?
const char* BUILTINS[] = {
    "+", "-", "*", "/", "%", "head", "tail", "list",
    "eval", "join", "cons", "def", "=", "lambda",
    "<=", "<", "==", ">=", ">"
};

bool is_builtin(char* name) {
    for (unsigned int i = 0; i < sizeof(BUILTINS) / sizeof(*BUILTINS); ++i) {
        if (strcmp(name, BUILTINS[i]) == 0) {
            return true;
        }
    }
    return false;
}

void builtins_init(lenv* env) {
    builtin_create(env, builtin_eval, "eval");

    // List functions
    builtin_create(env, builtin_list, "list");
    builtin_create(env, builtin_head, "head");
    builtin_create(env, builtin_tail, "tail");
    builtin_create(env, builtin_join, "join");
    builtin_create(env, builtin_cons, "cons");

    // Math functions
    builtin_create(env, builtin_add, "+");
    builtin_create(env, builtin_sub, "-");
    builtin_create(env, builtin_mul, "*");
    builtin_create(env, builtin_div, "/");
    builtin_create(env, builtin_mod, "%");

    // Conditions
    builtin_create(env, builtin_gt, ">");
    builtin_create(env, builtin_ge, ">=");
    builtin_create(env, builtin_lt, "<");
    builtin_create(env, builtin_le, "<=");
    builtin_create(env, builtin_eq, "==");
    builtin_create(env, builtin_ne, "!=");
    builtin_create(env, builtin_and, "and");
    builtin_create(env, builtin_or, "or");
    builtin_create(env, builtin_not, "not");
    builtin_create(env, builtin_if, "if");

    // Functions and scopes
    builtin_create(env, builtin_lambda, "lambda");
    builtin_create(env, builtin_def, "def");
    builtin_create(env, builtin_put, "=");
}

void builtin_create(lenv* env, lbuiltin func, char* name) {
    lval* key = lval_sym(name);
    lval* value = lval_func(func);

    lenv_put(env, key, value);
    lval_del(key); lval_del(value);
}

lval* builtin_add(lenv* env, lval* node) { return builtin_op(env, node, '+'); }
lval* builtin_sub(lenv* env, lval* node) { return builtin_op(env, node, '-'); }
lval* builtin_mul(lenv* env, lval* node) { return builtin_op(env, node, '*'); }
lval* builtin_div(lenv* env, lval* node) { return builtin_op(env, node, '/'); }
lval* builtin_mod(lenv* env, lval* node) { return builtin_op(env, node, '%'); }

lval* builtin_op(lenv* env, lval* node, char op) {
    UNUSED(env);

    char name[2]; name[0] = op; name[1] = '\0';

    // Ensure all arguments are numbers
    for (int i = 0; i < node->count; i++) {
        LASSERT_ARG_TYPE(name, node, i, LVAL_NUM);
    }

    lval* x = lval_pop(node, 0);

    // If no arguments and op == '-', do unary negation
    if ((op == '-') && node->count == 0) {
        x->num *= -1;
    }

    while (node->count > 0) {
        lval* y = lval_pop(node, 0);

        if      (op == '+') { x->num += y->num; }
        else if (op == '-') { x->num -= y->num; }
        else if (op == '*') { x->num *= y->num; }
        else if (op == '/' || op == '%') {
            if (y->num == 0) {
                lval_del(x); lval_del(y);
                x = lval_err("Division by zero");
                break;
            }

            switch (op) {
                case '%': x->num = fmodf(x->num, y->num); break;
                case '/': x->num /= y->num; break;
            }
        }

        lval_del(y);
    }

    lval_del(node);
    return x;
}

lval* builtin_gt(lenv* env, lval* node) { return builtin_ord(env, node, ">");  }
lval* builtin_ge(lenv* env, lval* node) { return builtin_ord(env, node, ">="); }
lval* builtin_lt(lenv* env, lval* node) { return builtin_ord(env, node, "<");  }
lval* builtin_le(lenv* env, lval* node) { return builtin_ord(env, node, "<="); }

lval* builtin_ord(lenv* env, lval* node, char* op) {
    UNUSED(env);

    // Check argument count
    LASSERT_MIN_ARG_COUNT(op, node, 2);

    // Ensure all arguments are numbers
    for (int i = 0; i < node->count; i++) {
        LASSERT_ARG_TYPE(op, node, i, LVAL_NUM);
    }

    lval* x = lval_pop(node, 0);
    PRECISION_INT result = 1;

    while (node->count > 0) {
        lval* y = lval_pop(node, 0);

        if      (strncmp(op, ">",  2) == 0) { result &= (x->num >  y->num); }
        else if (strncmp(op, ">=", 2) == 0) { result &= (x->num >= y->num); }
        else if (strncmp(op, "<",  2) == 0) { result &= (x->num < y->num); }
        else if (strncmp(op, "<=", 2) == 0) { result &= (x->num <= y->num); }

        x->num = y->num;

        lval_del(y);
    }

    lval_del(node);

    x->num = result;
    return x;
}

lval* builtin_eq(lenv* env, lval* node) { return builtin_cmp(env, node, "=="); }
lval* builtin_ne(lenv* env, lval* node) { return builtin_cmp(env, node, "!="); }

lval* builtin_cmp(lenv* env, lval* node, char* op) {
    UNUSED(env);

    LASSERT_ARG_COUNT(op, node, 2);

    lval* o1 = node->values[0];
    lval* o2 = node->values[1];

    PRECISION_INT result = 0;

    if      (strcmp(op, "==") == 0) { result =  lval_eq(o1, o2); }
    else if (strcmp(op, "!=") == 0) { result = !lval_eq(o1, o2); }
    else    { assertf(0, "Invalid op in builtin_cmp: %s", op); }

    lval_del(node);
    return lval_num(result);
}

lval* builtin_and(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("and", node, 2);
    LASSERT_ARG_TYPE("and", node, 0, LVAL_NUM);
    LASSERT_ARG_TYPE("and", node, 1, LVAL_NUM);

    PRECISION_INT result = node->values[0]->num && node->values[1]->num;
    lval_del(node);

    return lval_num(result);
}

lval* builtin_or(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("or", node, 2);
    LASSERT_ARG_TYPE("or", node, 0, LVAL_NUM);
    LASSERT_ARG_TYPE("or", node, 1, LVAL_NUM);

    PRECISION_INT result = node->values[0]->num || node->values[1]->num;
    lval_del(node);

    return lval_num(result);
}

lval* builtin_not(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("not", node, 1);
    LASSERT_ARG_TYPE("not", node, 0, LVAL_NUM);

    PRECISION_INT result = !node->values[0]->num;
    lval_del(node);

    return lval_num(result);
}

lval* builtin_if(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_MIN_ARG_COUNT("if", node, 2);
    LASSERT_MAX_ARG_COUNT("if", node, 3);
    LASSERT_ARG_TYPE("if", node, 0, LVAL_NUM);
    LASSERT_ARG_TYPE("if", node, 1, LVAL_QEXPR);

    if (node->count == 3) {
        LASSERT_ARG_TYPE("if", node, 2, LVAL_QEXPR);
    }

    lval* branch;

    // Check condition and choose branch
    if (node->values[0]->num) {
        branch = lval_pop(node, 1);
    } else if (node->count == 3) {
        branch = lval_pop(node, 2);
    } else {
        lval_del(node);
        return lval_sexpr();
    }

    lval_del(node);

    branch->type = LVAL_SEXPR;
    return eval(env, branch);
}

lval* builtin_head(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("head", node, 1);
    LASSERT_ARG_TYPE("head", node, 0, LVAL_QEXPR);
    LASSERT_ARG_NOT_EMPTY_LIST("head", node, 0);

    lval* qexpr = lval_take(node, 0);

    // Delete all elements that are not head and return
    while (qexpr->count > 1) {
        lval_del(lval_pop(qexpr, 1));
    }

    return qexpr;
}

lval* builtin_tail(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("tail", node, 1);
    LASSERT_ARG_TYPE("tail", node, 0, LVAL_QEXPR);
    LASSERT_ARG_NOT_EMPTY_LIST("tail", node, 0);

    lval* qexpr = lval_take(node, 0);
    lval_del(lval_pop(qexpr, 0));

    return qexpr;
}

lval* builtin_list(lenv* env, lval* node) {
    UNUSED(env);

    node->type = LVAL_QEXPR;
    return node;
}

lval* builtin_eval(lenv* env, lval* node) {
    LASSERT_ARG_COUNT("eval", node, 1);
    LASSERT_ARG_TYPE("eval", node, 0, LVAL_QEXPR);

    lval* qexpr = lval_take(node, 0);
    qexpr->type = LVAL_SEXPR;

    return eval(env, qexpr);
}

lval* builtin_join(lenv* env, lval* node) {
    UNUSED(env);

    for (int i = 0; i < node->count; i++) {
        LASSERT_ARG_TYPE("join", node, i, LVAL_QEXPR);
    }

    lval* qexpr = lval_pop(node, 0);
    while (node->count) {
        qexpr = lval_join(qexpr, lval_pop(node, 0));
    }

    lval_del(node);
    return qexpr;
}

lval* builtin_cons(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("cons", node, 2);
    LASSERT_ARG_TYPE("cons", node, 1, LVAL_QEXPR);

    lval* value = lval_pop(node, 0);
    lval* list = lval_pop(node, 0);

    lval* container = lval_qexpr();
    container = lval_add(container, value);
    container = lval_join(container, list);

    lval_del(node);
    return container;
}

lval* builtin_def(lenv* env, lval* node) { return builtin_var(env, node, DEF_GLOBAL); }
lval* builtin_put(lenv* env, lval* node) { return builtin_var(env, node, DEF_LOCAL); }

lval* builtin_var(lenv* env, lval* node, def_type type) {
    LASSERT_ARG_TYPE((type == DEF_GLOBAL) ? "def" : "=", node, 0, LVAL_QEXPR);

    lval* symbols = node->values[0];

    // Ensure all elements of first list are symbols and not builtins
    // TODO: Make more beautiful
    for (int i = 0; i < symbols->count; i++) {
        LASSERT(node, (symbols->values[i]->type == LVAL_SYM),
               "Function \"def\" cannot define non-symbol. Expected %s, got %s.",
               lval_str_type(LVAL_SYM), lval_str_type(symbols->values[i]->type));
        LASSERT(node, !is_builtin(symbols->values[i]->sym),
                "Cannot redefine builtin '%s'.", symbols->values[i]->sym);
    }

    // Check for correct number of symbols and values
    LASSERT(node, (symbols->count == node->count - 1),
            "Number of values is not number of symbols!");

    // Assign copies of values to symbols
    for (int i = 0; i < symbols->count; i++) {
        switch (type) {
            case DEF_LOCAL:  lenv_put(env, symbols->values[i], node->values[i + 1]);
                break;
            case DEF_GLOBAL: lenv_def(env, symbols->values[i], node->values[i + 1]);
                break;
            default: assertf(0, "Invalid def_type passed: %i", type);
        }
    }

    lval_del(node);
    return lval_sexpr();
}

lval* builtin_lambda(lenv* env, lval* node) {
    UNUSED(env);

    LASSERT_ARG_COUNT("lambda", node, 2);
    LASSERT_ARG_TYPE("lambda", node, 0, LVAL_QEXPR);
    LASSERT_ARG_TYPE("lambda", node, 1, LVAL_QEXPR);

    // Check if first Q-Expression only contains symbols
    lval* first_value = node->values[0];
    for (int i = 0; i < first_value->count; i++) {
        LASSERT(node, (first_value->values[i]->type == LVAL_SYM),
               "Cannot define non-symbol. Expected %s, got %s.",
               lval_str_type(LVAL_SYM), lval_str_type(first_value->values[i]->type));
    }

    lval* formals = lval_pop(node, 0);
    lval* body    = lval_pop(node, 0);
    lval_del(node);

    return lval_lambda(formals, body);
}

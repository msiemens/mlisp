#include <stdio.h>

#include "utils.h"
#include "lval.h"

#if defined DEBUG
    static long allocated_count = 0;
    static long deallocated_count = 0;
#endif


/// Wether a node is a list-like object.
bool is_list_like(lval* node) {
    return node->type == LVAL_SEXPR || node->type == LVAL_QEXPR;
}


/**
 * Return the string representation of a Q-Expr/S-Expr.
 *
 * Build and return the string representation of a Q-Expr/S-Expr.
 *
 * \param env   The environment from where the print is called.
 * \param node  The expression object to stringify.
 * \param open  The opening character: `(` or `{`.
 * \param close The closing character: `)` or `}`.
 *
 * \returns A char pointer containing the string. Has to be cleaned up!
 */
char* lval_str_expr(lenv* env, lval* node, char open, char close);

/**
 * Return the string representation of a function.
 *
 * Build and return the string representation of a function. If it's a builtin
 * function, the function returns `<function name>`.
 * If it's a lambda function, the function returns it's definition.
 *
 * \todo If lambda: Replace formals that are set with their values.
 *
 * \param env   The environment from where the print is called.
 * \param func  The function object to stringify.
 *
 * \returns A char pointer containing the string. Has to be cleaned up!
 */
char* lval_str_func(lenv* env, lval* func);

/**
 * Return the string representation of a string.
 *
 * Build and return the string representation of a string object.
 *
 * \param env   The environment from where the print is called.
 * \param node  The string object to stringify.
 *
 * \returns A char pointer containing the string. Has to be cleaned up!
 */
char* lval_str_str(lenv* env, lval* node);


lval* lval_new(void) {
#if defined DEBUG
    allocated_count += 1;
#endif
    return xmalloc(LVAL_SIZE);
}

lval* lval_sexpr(void) {
    lval* node = lval_new();
    node->type = LVAL_SEXPR;
    node->count = 0;
    node->values = NULL;

    return node;
}

lval* lval_qexpr(void) {
    lval* node = lval_new();
    node->type = LVAL_QEXPR;
    node->count = 0;
    node->values = NULL;

    return node;
}

lval* lval_sym(char* symbol) {
    ASSERT_NOT_NULL(symbol);

    lval* node = lval_new();
    node->type = LVAL_SYM;
    node->sym = strdup(symbol);

    return node;
}

lval* lval_num(PRECISION_FLOAT value) {
    lval* node = lval_new();
    node->type = LVAL_NUM;
    node->num = value;

    return node;
}

lval* lval_str(char* str) {
    ASSERT_NOT_NULL(str);

    lval* node = lval_new();
    node->type = LVAL_STR;
    node->str = strdup(str);

    return node;
}

lval* lval_err(char* fmt, ...) {
    ASSERT_NOT_NULL(fmt);

    lval* node = lval_new();
    node->type = LVAL_ERR;

    va_list va;
    va_start(va, fmt);

    size_t required_size = xvsnprintf(NULL, 0, fmt, va);
    node->err = xmalloc(required_size + 1);
    xvsnprintf(node->err, required_size + 1, fmt, va);

    va_end(va);

    return node;
}

lval* lval_func(lbuiltin func) {
    lval* node = lval_new();
    node->type = LVAL_FUNC;
    node->builtin = func;

    return node;
}

lval* lval_lambda(lval* formals, lval* body) {
    ASSERT_NOT_NULL(formals);
    ASSERT_NOT_NULL(body);

    lval* node = lval_new();
    node->type = LVAL_FUNC;

    node->builtin = NULL;
    node->env = lenv_new();
    node->formals = formals;
    node->body = body;

    return node;
}

void lval_del(lval* node) {
    ASSERT_NOT_NULL(node);

#if defined DEBUG
    deallocated_count += 1;
#endif

    switch(node->type) {
        case LVAL_NUM: break;
        case LVAL_FUNC:
            if (!node->builtin) {
                lenv_del(node->env);
                lval_del(node->formals);
                lval_del(node->body);
            }
            break;

        // Types with strings
        case LVAL_ERR: xfree(node->err); break;
        case LVAL_SYM: xfree(node->sym); break;
        case LVAL_STR: xfree(node->str); break;

        // Sexpr: Delete all elements inside
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            for_item(node, {
                lval_del(item);
            })

            // Free memory allocated to contain the pointers
            if (node->values) {
                xfree(node->values);
            }
            break;
        default:
            ASSERTF(0, "Invalid lval type: %i", node->type);
    }

#if defined DEBUG
    memset(node, 0xEE, LVAL_SIZE);
#endif
    xfree(node);
}

lval* lval_copy(lval* node) {
    ASSERT_NOT_NULL(node);

    lval* copy = lval_new();
    copy->type = node->type;

    switch (node->type) {
        // Copy functions and numbers directly
        case LVAL_NUM:
            copy->num = node->num;
            break;
        case LVAL_FUNC:
            if (node->builtin) {
                copy->builtin = node->builtin;
            } else {
                copy->builtin = NULL;
                copy->env     = lenv_copy(node->env);
                copy->formals = lval_copy(node->formals);
                copy->body    = lval_copy(node->body);
            }
            break;

        // Copy strings
        case LVAL_ERR: copy->err = strdup(node->err); break;
        case LVAL_SYM: copy->sym = strdup(node->sym); break;
        case LVAL_STR: copy->str = strdup(node->str); break;

        // Copy lists by copying each subexpression
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            copy->count  = node->count;
            copy->values = xmalloc(LVAL_PTR_SIZE * node->count);

            for_item(node, {
                copy->values[i] = lval_copy(item);
            })
            break;
        default:
            ASSERTF(0, "Invalid lval type: %i", node->type);
    }

    return copy;
}

bool lval_eq(lval* x, lval* y) {
    ASSERT_NOT_NULL(x);
    ASSERT_NOT_NULL(y);

    if (x->type != y->type) {
        return false;
    }

    switch (x->type) {
        case LVAL_NUM: return fcmp(x->num, y->num);

        case LVAL_ERR: return (strcmp(x->err, y->err) == false);
        case LVAL_SYM: return (strcmp(x->sym, y->sym) == false);
        case LVAL_STR: return (strcmp(x->str, y->str) == false);

        case LVAL_FUNC:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body);
            }

        case LVAL_SEXPR:
        case LVAL_QEXPR:
            if (x->count != y->count) {
                return false;
            }
            for_item(x, {
                if (!lval_eq(x->values[i], y->values[i])) {
                    return false;
                }
            });

            return true;
    }

    return false;
}

lval* lval_add(lval* container, lval* value) {
    ASSERT_LIST_LIKE(container);

    container->count++;
    container->values = xrealloc(container->values,
                                 LVAL_PTR_SIZE * container->count);
    container->values[container->count - 1] = value;

    return container;
}

lval* lval_join(lval* first, lval* second) {
    ASSERT_LIST_LIKE(first);
    ASSERT_LIST_LIKE(second);

    // For each value in 'second', add it to 'first'
    while (second->count) {
        first = lval_add(first, lval_pop(second, 0));
    }

    lval_del(second);
    return first;
}

lval* lval_pop(lval* container, size_t index) {
    ASSERT_LIST_LIKE(container);
    ASSERT_ARG(container, container->count > 0, "count is %i (<= 0)", container->count);
    ASSERT_ARG(index, index <= container->count, "is %i (< container->count = %i)", index, container->count);

    lval* item = container->values[index];

    // Shift memory following the item at 'index' over the top of it
    memmove(&container->values[index], &container->values[index + 1],
            LVAL_PTR_SIZE * (container->count - index - 1));

    container->count--;

    // Reallocate the memory used
    container->values = xrealloc(container->values, LVAL_PTR_SIZE * container->count);

    return item;
}

lval* lval_take(lval* container, size_t index) {
    lval* item = lval_pop(container, index);
    lval_del(container);

    return item;
}

char* lval_str_type(lval_type type) {
    switch (type) {
        case LVAL_SEXPR: return "S-Expression";
        case LVAL_QEXPR: return "Q-Expression";
        case LVAL_SYM:   return "symbol";
        case LVAL_STR:   return "string";
        case LVAL_NUM:   return "number";
        case LVAL_ERR:   return "error";
        case LVAL_FUNC:  return "function";
        default:         return "unknown";
    }
}

char* lval_str_expr(lenv* env, lval* node, char open, char close) {
    size_t buffer_length = 2; // open char + \0

    char* buffer = xmalloc(buffer_length);
    memset(buffer, 0, buffer_length);

    buffer[0] = open;

    for_item(node, {
        char* tmp = lval_repr(env, item);
        unsigned int tmp_len = strlen(tmp);

        buffer_length += tmp_len;
        buffer = strappend(buffer, tmp, buffer_length);
        xfree(tmp);

        // Print space unless last element
        if (i != (node->count - 1)) {
            buffer_length += 1;
            buffer = strappend(buffer, " ", buffer_length);
        }
    });

    buffer_length += 1;
    buffer = xrealloc(buffer, buffer_length);
    buffer[buffer_length - 2] = close;
    buffer[buffer_length - 1] = '\0';

    return buffer;
}

char* lval_str_func(lenv* env, lval* func) {
    if (func->builtin) {
        char* sym = lenv_func_name(env, func->builtin);
        if (sym) {
            return xsprintf("<function %s>", sym);
        } else {
            return strdup("<builtin>");
        }
    } else {
        char* str_formals = lval_repr(env, func->formals);
        char* str_body    = lval_repr(env, func->body);

        char* buffer = xsprintf("(lambda %s %s)", str_formals, str_body);

        xfree(str_formals);
        xfree(str_body);

        return buffer;
    }
}

char* lval_str_str(lenv* env, lval* node) {
    UNUSED(env);
    return strdup(node->str);
}

char* lval_to_str(lenv* env, lval* node) {
    ASSERT_NOT_NULL(env);
    ASSERT_NOT_NULL(node);

    switch(node->type) {
        case LVAL_SEXPR: return lval_str_expr(env, node, '(', ')');
        case LVAL_QEXPR: return lval_str_expr(env, node, '{', '}');
        case LVAL_FUNC:  return lval_str_func(env, node);
        case LVAL_STR:   return lval_str_str(env, node);
        case LVAL_SYM:   return xsprintf("%s", node->sym);
        case LVAL_NUM:   return xsprintf("%g", node->num);
        case LVAL_ERR:   return xsprintf("Error: %s", node->err);
        default:         ASSERTF(0, "Encountered invalid lval type: %i", node->type);
    }

    return NULL;
}

char* lval_repr(lenv* env, lval* node) {
    ASSERT_NOT_NULL(env);
    ASSERT_NOT_NULL(node);

    if (node->type == LVAL_STR) {
        char* escaped = NULL;
        escaped = strdup(node->str);
        escaped = mpcf_escape(escaped);

        char* quoted = xsprintf("\"%s\"", escaped);
        xfree(escaped);

        return quoted;
    } else {
        return lval_to_str(env, node);
    }
}

// Forward declaration
char* lenv_func_name(lenv* env, lbuiltin func);

void lval_print(lenv* env, lval* node) {
    ASSERT_NOT_NULL(env);
    ASSERT_NOT_NULL(node);

    char* repr = lval_to_str(env, node);
    printf("%s", repr);
    xfree(repr);
}

void lval_println(lenv* env, lval* node) {
    lval_print(env, node); putchar('\n');
}

#if defined DEBUG
    void lval_print_stats(void) {
        printf("Number of allocated objects:   %ld\n", allocated_count);
        printf("Number of deallocated objects: %ld\n", deallocated_count);
        printf("Number of alive objects:       %ld\n", allocated_count - deallocated_count);
    }
#endif

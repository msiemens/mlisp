#include <stdio.h>

#include "config.h"
#include "lval.h"

lval* lval_new(void) {
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
    lval* node = lval_new();
    node->type = LVAL_SYM;
    node->sym = mstrcpy(symbol, node->sym);

    return node;
}

lval* lval_num(PRECISION value) {
    lval* node = lval_new();
    node->type = LVAL_NUM;
    node->num = value;

    return node;
}

lval* lval_err(char* fmt, ...) {
    lval* node = lval_new();
    node->type = LVAL_ERR;

    va_list va;
    va_start(va, fmt);

    size_t required_size = vsnprintf(NULL, 0, fmt, va);
    node->err = xmalloc(required_size + 1);
    vsnprintf(node->err, required_size + 1, fmt, va);

    //node->err = xrealloc(node->err, strlen(node->err) + 1);

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
    lval* node = lval_new();
    node->type = LVAL_FUNC;

    node->builtin = NULL;
    node->env = lenv_new();
    node->formals = formals;
    node->body = body;

    return node;
}

void lval_del(lval* node) {
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

        // Sexpr: Delete all elements inside
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            for (int i = 0; i < node->count; i++) {
                lval_del(node->values[i]);
            }

            // Free memory allocated to contain the pointers
            if (node->values) {
                xfree(node->values);
            }
            break;
    }

    xfree(node);
}

lval* lval_copy(lval* node) {
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

        // Copy strings using malloc and strcpy
        case LVAL_ERR: copy->err = mstrcpy(node->err, copy->err); break;
        case LVAL_SYM: copy->sym = mstrcpy(node->sym, copy->sym); break;

        // Copy lists by copying each subexpression
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            copy->count  = node->count;
            copy->values = xmalloc(LVAL_PTR_SIZE * node->count);
            for (int i = 0; i < node->count; i++) {
                copy->values[i] = lval_copy(node->values[i]);
            }
            break;
    }

    return copy;
}

int lval_eq(lval* x, lval* y) {
    if (x->type != y->type) {
        return 0;
    }

    switch (x->type) {
       case LVAL_NUM: return (x->num == y->num);

        case LVAL_ERR: return (strcmp(x->err, y->err) == 0);
        case LVAL_SYM: return (strcmp(x->sym, y->sym) == 0);

        case LVAL_FUNC:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body);
            }

        case LVAL_SEXPR:
        case LVAL_QEXPR:
            if (x->count != y->count) {
                return 0;
            }
            for (int i = 0; i < x->count; i++) {
                if (!lval_eq(x->values[i], y->values[i])) {
                    return 0;
                }
            }

            return 1;

    }

    return 0;
}

lval* lval_add(lval* container, lval* value) {
    assertf(container->type == LVAL_QEXPR ||
            container->type == LVAL_SEXPR, "lval_add passed a non-container");

    container->count++;
    container->values = xrealloc(container->values,
                                LVAL_PTR_SIZE * container->count);
    container->values[container->count - 1] = value;

    return container;
}

lval* lval_join(lval* first, lval* second) {
    // For each value in 'second', add it to 'first'
    while (second->count) {
        first = lval_add(first, lval_pop(second, 0));
    }

    lval_del(second);
    return first;
}

lval* lval_pop(lval* node, int index) {
    lval* item = node->values[index];

    // Shift memory following the item at 'index' over the top of it
    memmove(&node->values[index], &node->values[index + 1],
            LVAL_PTR_SIZE * (node->count - index - 1));

    node->count--;

    // Reallocate the memory used
    node->values = xrealloc(node->values, LVAL_PTR_SIZE * node->count);

    return item;
}

lval* lval_take(lval* node, int index) {
    lval* item = lval_pop(node, index);
    lval_del(node);

    return item;
}

lval* lval_read_num(mpc_ast_t* tree) {
    PRECISION val = strtod(tree->contents, NULL);
    if (errno != ERANGE) {
        return lval_num(val);
    } else {
        return lval_err("Invalid number: %s", tree->contents);
    }
}

lval* lval_read(mpc_ast_t* tree) {
    if      (strstr(tree->tag, "number")) { return lval_read_num(tree); }
    else if (strstr(tree->tag, "symbol")) { return lval_sym(tree->contents); }

    lval* expr = NULL;
    // If root (">") or sexpr or qexpr, create empty list
    if (strcmp(tree->tag, ">") == 0)     { expr = lval_sexpr(); }
    else if (strstr(tree->tag, "sexpr")) { expr = lval_sexpr(); }
    else if (strstr(tree->tag, "qexpr")) { expr = lval_qexpr(); }
    else { assertf(0, "Unkown tag containing elements"); }

    // Fill this list with any valid expression contained within
    for (int i = 0; i < tree->children_num; i++) {
        if      (strstr(tree->children[i]->tag, "comment"))     { continue; }
        else if (strcmp(tree->children[i]->contents, "(") == 0) { continue; }
        else if (strcmp(tree->children[i]->contents, ")") == 0) { continue; }
        else if (strcmp(tree->children[i]->contents, "{") == 0) { continue; }
        else if (strcmp(tree->children[i]->contents, "}") == 0) { continue; }
        else if (strcmp(tree->children[i]->tag, "regex") == 0)  { continue; }

        expr = lval_add(expr, lval_read(tree->children[i]));
    }

    return expr;
}

char* lval_str_type(int type) {
    switch (type) {
        case LVAL_SEXPR: return "S-Expression";
        case LVAL_QEXPR: return "Q-Expression";
        case LVAL_SYM:   return "symbol";
        case LVAL_NUM:   return "number";
        case LVAL_ERR:   return "error";
        case LVAL_FUNC:  return "function";
        default:         return "unknown";
    }
}

char* lval_str_expr(lval* node, char open, char close) {
    size_t buffer_length = 2; // open char + \0

    char* buffer = xmalloc(buffer_length);
    memset(buffer, 0, buffer_length);

    buffer[0] = open;

    for (int i = 0; i < node->count; i++) {
        char* tmp = lval_str(node->values[i]);
        unsigned int tmp_len = strlen(tmp);

        buffer_length += tmp_len;
        buffer = strappend(buffer, tmp, buffer_length);
        xfree(tmp);

        // Print space unless last element
        if (i != (node->count - 1)) {
            buffer_length += 1;
            buffer = strappend(buffer, " ", buffer_length);
        }
    }

    buffer_length += 1;
    buffer = xrealloc(buffer, buffer_length);
    buffer[buffer_length - 2] = close;
    buffer[buffer_length - 1] = '\0';

    return buffer;
}

char* lval_str_func(lval* func) {
    if (func->builtin) {
        return strdup("<builtin>");
    } else {
        char* fmt = "(lambda %s %s)";
        char* str_formals = lval_str(func->formals);
        char* str_body    = lval_str(func->body);

        size_t size = snprintf(NULL, 0, fmt, str_formals, str_body);
        char* buffer = xmalloc(size + 1);
        snprintf(buffer, size + 1, "(lambda %s %s)", str_formals, str_body);

        xfree(str_formals);
        xfree(str_body);

        return buffer;
    }
}

char* lval_str(lval* node) {
    switch(node->type) {
        case LVAL_SEXPR: return lval_str_expr(node, '(', ')');
        case LVAL_QEXPR: return lval_str_expr(node, '{', '}');
        case LVAL_FUNC:  return lval_str_func(node);
        case LVAL_SYM:   return xsprintf("%s", node->sym);
        case LVAL_NUM:   return xsprintf("%g", node->num);
        case LVAL_ERR:   return xsprintf("Error: %s", node->err);
        default:         assertf(0, "Encountered invalid lval type: %i", node->type);
    }

    return NULL;
}

// Forward declaration
char* lenv_func_name(lenv* env, lbuiltin func);

void lval_print(lenv* env, lval* node) {
    if (node->type == LVAL_FUNC && node->builtin) {
        char* sym = lenv_func_name(env, node->builtin);
        if (sym) {
            printf("<function '%s'>", sym);
            return;
        }
    }

    char* repr = lval_str(node);
    printf("%s", repr);
    xfree(repr);
}

void lval_println(lenv* env, lval* node) {
    lval_print(env, node); putchar('\n');
}

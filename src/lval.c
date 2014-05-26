#include <stdio.h>

#include "config.h"
#include "lval.h"

lval* lval_sexpr(void) {
    lval* node = NEW_LVAL;
    node->type = LVAL_SEXPR;
    node->count = 0;
    node->values = NULL;

    return node;
}

lval* lval_qexpr(void) {
    lval* node = NEW_LVAL;
    node->type = LVAL_QEXPR;
    node->count = 0;
    node->values = NULL;

    return node;
}

lval* lval_sym(char* symbol) {
    lval* node = NEW_LVAL;
    node->type = LVAL_SYM;
    node->sym = malloc(strlen(symbol) + 1);
    strcpy(node->sym, symbol);

    return node;
}

lval* lval_num(PRECISION value) {
    lval* node = NEW_LVAL;
    node->type = LVAL_NUM;
    node->num = value;

    return node;
}

lval* lval_err(char* message) {
    lval* node = NEW_LVAL;
    node->type = LVAL_ERR;
    node->err = malloc(strlen(message) + 1);
    strcpy(node->err, message);

    return node;
}

lval* lval_func(lbuiltin func) {
    lval* node = NEW_LVAL;
    node->type = LVAL_FUNC;
    node->func = func;

    return node;
}

void lval_del(lval* node) {
    switch(node->type) {
        case LVAL_NUM: break;
        case LVAL_FUNC: break;

        // Types with strings
        case LVAL_ERR: free(node->err); break;
        case LVAL_SYM: free(node->sym); break;

        // Sexpr: Delete all elements inside
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            for (int i = 0; i < node->count; i++) {
                lval_del(node->values[i]);
            }

            // Free memory allocated to contain the pointers
            free(node->values);
            break;
    }

    free(node);
}

lval* lval_copy(lval* node) {
    lval* copy = NEW_LVAL;
    copy->type = node->type;

    switch (node->type) {
        // Copy functions and numbers directly
        case LVAL_NUM:  copy->num  = node->num;  break;
        case LVAL_FUNC: copy->func = node->func; break;

        // Copy strings using malloc and strcpy
        case LVAL_ERR: copy->err = STRCPY(node->err, copy->err); break;
        case LVAL_SYM: copy->err = STRCPY(node->sym, copy->sym); break;

        // Copy lists by copying each subexpression
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            copy->count  = node->count;
            copy->values = malloc(LVAL_PTR_SIZE * node->count);
            for (int i = 0; i < node->count; i++) {
                copy->values[i] = lval_copy(node->values[i]);
            }
            break;
    }

    return copy;
}

lval* lval_add(lval* container, lval* value) {
    container->count++;
    container->values = realloc(container->values,
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
    node->values = realloc(node->values, LVAL_PTR_SIZE * node->count);

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
        return lval_err("invalid number");
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

    // Fill this list with any valid expression contained within
    for (int i = 0; i < tree->children_num; i++) {
        if      (strcmp(tree->children[i]->contents, "(") == 0) { continue; }
        else if (strcmp(tree->children[i]->contents, ")") == 0) { continue; }
        else if (strcmp(tree->children[i]->contents, "{") == 0) { continue; }
        else if (strcmp(tree->children[i]->contents, "}") == 0) { continue; }
        else if (strcmp(tree->children[i]->tag, "regex") == 0)  { continue; }

        expr = lval_add(expr, lval_read(tree->children[i]));
    }

    return expr;
}

void lval_expr_print(lval* node, char open, char close) {
    putchar(open);

    for (int i = 0; i < node->count; i++) {
        lval_print(node->values[i]);

        // Print space unless last element
        if (i != (node->count - 1)) {
            putchar(' ');
        }
    }

    putchar(close);
}

void lval_print(lval* node) {
    switch (node->type) {
        case LVAL_SEXPR: lval_expr_print(node, '(', ')'); break;
        case LVAL_QEXPR: lval_expr_print(node, '{', '}'); break;
        case LVAL_SYM:   printf("%s", node->sym); break;
        case LVAL_NUM:   printf("%g", node->num); break;
        case LVAL_ERR:   printf("Error: %s", node->err); break;
        case LVAL_FUNC:  printf("<function>"); break;
        default:         printf("!!! THIS SHOULD NEVER HAPPEN (INVALID TYPE) !!!");
    }
}

void lval_println(lval* node) {
    lval_print(node); putchar('\n');
}

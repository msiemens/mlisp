#include "utils.h"
#include "parser.h"
#include "eval.h"


mpc_parser_t* number;
mpc_parser_t* symbol;
mpc_parser_t* string;
mpc_parser_t* comment;
mpc_parser_t* sexpr;
mpc_parser_t* qexpr;
mpc_parser_t* expr;
mpc_parser_t* lispy;


// TODO: Docs
lval* parse_num(mpc_ast_t* tree);

lval* parse_str(char* contents);


void parser_init(void) {
    number = mpc_new("number");
    symbol = mpc_new("symbol");
    string = mpc_new("string");
    comment = mpc_new("comment");
    sexpr = mpc_new("sexpr");
    qexpr = mpc_new("qexpr");
    expr = mpc_new("expr");
    lispy = mpc_new("lispy");

    // Define them with the following Language
    mpc_err_t* err = mpca_lang(MPCA_LANG_DEFAULT, /*__GRAMMAR__*/,
                               number, symbol, string, comment, sexpr, qexpr, expr, lispy);

    if (err != NULL) {
        mpc_err_print(err);
        mpc_err_delete(err);
        exit(1);
    }
}

mpc_parser_t* parser_get(void) {
    return lispy;
}

void parser_cleanup(void) {
    mpc_cleanup(7, number, symbol, string, comment,  sexpr, qexpr, expr, lispy);
}

lval* parse_tree(mpc_ast_t* tree) {
    // assert_not_null(tree);  // FIXME: Comment in

    if (strstr(tree->tag, "number")) {
        return parse_num(tree);
    } else if (strstr(tree->tag, "symbol")) {
        return lval_sym(tree->contents);
    } else if (strstr(tree->tag, "string")) {
        return parse_str(tree->contents);
    }

    lval* node = NULL;
    // If root (">") or sexpr or qexpr, create empty list
    if (strcmp(tree->tag, ">") == 0) {
        node = lval_sexpr();
    } else if (strstr(tree->tag, "sexpr")) {
        node = lval_sexpr();
    } else if (strstr(tree->tag, "qexpr")) {
        node = lval_qexpr();
    } else {
        ASSERTF(0, "Unkown tag containing elements");
    }

    // Fill this list with any valid expression contained within
    for (int i = 0; i < tree->children_num; i++) {
        if (strstr(tree->children[i]->tag, "comment")) {
            continue;
        } else if (strcmp(tree->children[i]->contents, "(") == 0) {
            continue;
        } else if (strcmp(tree->children[i]->contents, ")") == 0) {
            continue;
        } else if (strcmp(tree->children[i]->contents, "{") == 0) {
            continue;
        } else if (strcmp(tree->children[i]->contents, "}") == 0) {
            continue;
        } else if (strcmp(tree->children[i]->tag, "regex") == 0) {
            continue;
        }

        node = lval_add(node, parse_tree(tree->children[i]));
    }

    return node;
}

lval* parse_num(mpc_ast_t* tree) {
    PRECISION_FLOAT val = strtod(tree->contents, NULL);
    return lval_num(val);
}

lval* parse_str(char* contents) {
    size_t len = strlen(contents);

    // Cut off the final quote character
    contents[len - 1] = '\0';

    // Copy the string leaving out the first quote character
    char* unescaped = NULL;
    unescaped = strdup(contents + 1);

    // Unescape the string
    unescaped = mpcf_unescape(unescaped);

    lval* node = lval_str(unescaped);

    xfree(unescaped);
    return node;
}

bool parse(char* filename, char* str, lenv* env, lval** result, mpc_err_t** parser_error) {
    mpc_result_t r;
    if (mpc_parse(filename, str, parser_get(), &r)) {
        *result = eval(env, parse_tree(r.output));

        mpc_ast_delete(r.output);

        return true;
    } else {
        *parser_error = r.error;

        return false;
    }
}

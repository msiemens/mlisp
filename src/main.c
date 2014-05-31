#include "config.h"
#include "grammar.h"
#include "lval.h"
#include "lenv.h"
#include "eval.h"
#include "builtin.h"
#include "utils.h"

#ifdef _WIN32

// Do not warn about unused argument
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

char* readline(char* prompt) {
    // Print prompt
    fputs(prompt, stdout);

    // Read user input
    unsigned int len_max = 2048;
    unsigned int current_size = 0;

    char* input = xmalloc(len_max);
    current_size = len_max;

    int c = EOF;
    unsigned int i = 0;

    while ((c = getchar()) != '\n' && c != EOF) {
        input[i++] = (char) c;

        if (current_size == i) {
            current_size = i + len_max;
            input = xrealloc(input, current_size);
        }
    }
    input[i] = '\0';

    // Trim string
    char* cpy = xmalloc(i + 1);
    strncpy(cpy, input, i + 1);
    FREE(input);

    return cpy;
}

void add_history(char* unused) {}

#pragma GCC diagnostic pop

#else

#include <editline/readline.h>
#include <editline/history.h>

#endif

int main(void) {
    // Welcome message
    puts("MLisp Version 0.0.0.0.2");
    puts("Enter 'quit' to exit\n");

    // Initialization
    mpc_parser_t* Lispy = parser_get();
    lenv* env = lenv_new();
    builtins_init(env);

    while (1) {
        char* input = readline(">>> ");
        add_history(input);

        if (strstr(input, "exit") || strstr(input, "quit")) {
            puts("Bye!");
            FREE(input);
            break;
        }

        // Attempt to parse the user input
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            // On success evaluate the AST, print the result and delete the AST
            lval* result = eval(env, lval_read(r.output));
            lval_println(env, result);
            lval_del(result);
            mpc_ast_delete(r.output);
        } else {
            // Otherwise print and delete the Error
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        FREE(input);
    }

    lenv_del(env);

    // Undefine and delete our parsers
    parser_cleanup();
}


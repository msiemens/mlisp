#include "mlisp.h"

#ifdef _WIN32

// Do not warn about unused argument
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"

void add_history(char* unused) {}

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
    xfree(input);

    return cpy;
}

#pragma GCC diagnostic pop

#else

#include <readline/readline.h>
#include <readline/history.h>

#endif

int count_char(char* s, char c) {
    const char *p = s;
    int count = 0;

    do {
        if (*p == c)
            count++;
    } while (*(p++));

    return count;
}

char* read_input() {
    char* input = readline(">>> ");

    int braces_balanced = 0;

    do {
        int parens_l = count_char(input, '(');
        int parens_r = count_char(input, ')');
        int braces_l = count_char(input, '{');
        int braces_r = count_char(input, '}');

        braces_balanced = (parens_r == parens_l && braces_r == braces_l);
        if (!braces_balanced) {
            // Read next line and append it
            char* continuation = readline("... ");
            if (strlen(continuation) == 0) { return input; }
            input = strappend(input, continuation, strlen(input) + strlen(continuation) + 1);
            free(continuation);
        }
    } while (!braces_balanced);

    return input;
}

int main(int argc, char** argv) {
    // Initialization
    //mpc_parser_t* Lispy = parser_get();
    lenv* env = lenv_new();
    builtins_init(env);

    if (argc >= 2) {
        // Loop over file names
        for (int i = 1; i < argc; i++) {
            lval* args   = lval_add(lval_sexpr(), lval_str(argv[i]));
            lval* result = builtin_load(env, args);

            if (result->type == LVAL_ERR) {
                lval_println(env, result);
            }
            lval_del(result);
        }
    } else {
        // Welcome message
        puts("MLisp Version 0.1");
        puts("Enter 'quit' to exit\n");

        while (1) {
            char* input = read_input();
            add_history(input);

            if (strstr(input, "exit") || strstr(input, "quit")) {
                puts("Bye!");
                xfree(input);
                break;
            }

            lval* result = NULL;
            mpc_err_t* parser_error;
            memset(&parser_error, 0, sizeof(parser_error));

            if (parse("<stdin>", input, env, &result, &parser_error)) {
                if (!(result->type == LVAL_SEXPR && result->count == 0)) {
                    char* repr = lval_repr(env, result);
                    printf("%s\n", repr);
                    xfree(repr);
                }

                lval_del(result);
            } else {
                mpc_err_print(parser_error);
                mpc_err_delete(parser_error);
            }

            xfree(input);
        }
    }

    lenv_del(env);

    // Undefine and delete our parsers
    parser_cleanup();
}


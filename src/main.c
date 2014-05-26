#include "config.h"
#include "grammar.h"
#include "lval.h"
#include "lenv.h"
#include "eval.h"
#include "builtin.h"

#ifdef _WIN32

// Do not warn about unused argument
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static char buffer[2048];

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

void add_history(char* unused) {}

#pragma GCC diagnostic pop

#else

#include <editline/readline.h>
#include <editline/history.h>

#endif

int main(void) {
    // Get parser
    mpc_parser_t* Lispy    = parser_get();

    // Welcome message
    puts("MLisp Version 0.0.0.0.2");
    puts("Enter 'quit' to exit\n");

    lenv* env = lenv_new();
    builtins_init(env);

    while (1) {

        char* input = readline(">>> ");
        add_history(input);

        if (strstr(input, "exit") || strstr(input, "quit")) {
            puts("Bye!");
            free(input);
            break;
        }

        // Attempt to parse the user input
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            // On success evaluate the AST, print the result and delete the AST
            lval* result = eval(env, lval_read(r.output));
            lval_println(result);
            lval_del(result);
            mpc_ast_delete(r.output);
        } else {
            // Otherwise print and delete the Error
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
  }

  lenv_del(env);

  // Undefine and delete our parsers
  parser_cleanup();
}


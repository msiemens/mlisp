#include "lenv.h"

lenv* lenv_new(void) {
    lenv* env = NEW_LENV;
    env->count = 0;
    env->symbols = NULL;
    env->values = NULL;

    return env;
}

void lenv_del(lenv* env) {
    for (int i = 0; i < env->count; i++) {
        free(env->symbols[i]);
        lval_del(env->values[i]);
    }

    free(env->symbols);
    free(env->values);
    free(env);
}

lval* lenv_get(lenv* env, lval* key) {
    for (int i = 0; i < env->count; i++) {
        // Check if the stored string matches the symbol string
        // If it does, return a copy of the value
        if (strcmp(env->symbols[i], key->sym) == 0) {
            return lval_copy(env->values[i]);
        }
    }

    return lval_err("Unbound symbol!");
}

void lenv_put(lenv* env, lval* key, lval* value) {
    // Check, if variable already exists
    for (int i = 0; i < env->count; i++) {
        // If variable is found, delete item at that position and replace with
        // variable supplied by the user
        if (strcmp(env->symbols[i], key->sym) == 0) {
            lval_del(env->values[i]);
            env->values[i] = lval_copy(value);
            env->symbols[i] = RSTRCPY(key->sym, env->symbols[i]);
        }
    }

    // Allocate space for new entry
    env->count++;
    env->values = realloc(env->values, LENV_PTR_SIZE * env->count);
    env->symbols = realloc(env->symbols, CHAR_PTR_SIZE * env->count);

    // Copy contents of lval and symbol string into new location
    env->values[env->count - 1] = lval_copy(value);
    env->symbols[env->count - 1] = STRCPY(key->sym, env->symbols[env->count - 1])
}

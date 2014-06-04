#include "lenv.h"


#define LENV_GET(env, i) \
    env->values[i]


lenv* lenv_new(void) {
    lenv* env = xmalloc(LENV_SIZE);
    env->parent = NULL;
    env->count = 0;
    env->symbols = NULL;
    env->values = NULL;

    return env;
}

void lenv_del(lenv* env) {
    for (int i = 0; i < env->count; i++) {
        xfree(env->symbols[i]);
        lval_del(env->values[i]);
    }

    if (env->symbols) { xfree(env->symbols); }
    if (env->values)  { xfree(env->values);  }
    xfree(env);
}

lenv* lenv_copy(lenv* env) {
    lenv* copy = xmalloc(LENV_SIZE);
    copy->parent  = env->parent;
    copy->count   = env->count;
    copy->symbols = xmalloc(CHAR_PTR_SIZE * copy->count);
    copy->values  = xmalloc(LVAL_PTR_SIZE * copy->count);

    for (int i = 0; i < env->count; i++) {
        copy->symbols[i] = strdup(env->symbols[i]);
        copy->values[i]  = lval_copy(env->values[i]);
    }

    return copy;
}

lval* lenv_get(lenv* env, lval* key) {
    for (int i = 0; i < env->count; i++) {
        // Check if the stored string matches the symbol string
        // If it does, return a copy of the value
        if (strcmp(env->symbols[i], key->sym) == 0) {
            return lval_copy(env->values[i]);
        }
    }

    if (env->parent) {
        return lenv_get(env->parent, key);
    } else {
        return lval_err("Unbound symbol: '%s'", key->sym);
    }
}

void lenv_put(lenv* env, lval* key, lval* value) {
    // Check, if variable already exists
    for (int i = 0; i < env->count; i++) {
        // If variable is found, delete item at that position and replace with
        // variable supplied by the user
        if (strcmp(env->symbols[i], key->sym) == 0) {
            lval_del(env->values[i]);
            env->values[i] = lval_copy(value);
            return;
        }
    }

    // Allocate space for new entry
    env->count++;
    env->values  = xrealloc(env->values,  LVAL_PTR_SIZE * env->count);
    env->symbols = xrealloc(env->symbols, CHAR_PTR_SIZE * env->count);

    // Copy contents of lval and symbol string into new location
    env->values[env->count - 1]  = lval_copy(value);
    env->symbols[env->count - 1] = strdup(key->sym);
}

void lenv_def(lenv* env, lval* key, lval* value) {
    // Seek parent
    while (env->parent) {
        env = env->parent;
    }

    lenv_put(env, key, value);
}

char* lenv_func_name(lenv* env, lbuiltin func) {
    for (int i = 0; i < env->count; i++) {
        if (env->values[i]->type == LVAL_FUNC && env->values[i]->builtin == func) {
            return env->symbols[i];
        }
    }

    return NULL;
}

#include "lenv.h"


lenv* lenv_new(void) {
    lenv* env = xmalloc(sizeof(lenv));
    env->parent = NULL;
    env->values = hash_new();

    return env;
}

void lenv_del(lenv* env) {
    hash_each_val(env->values, {
        lval_del(val);
    });
    hash_free(env->values);

    xfree(env);
}

lenv* lenv_copy(lenv* env) {
    lenv* copy = lenv_new();

    // FIXME: I suspect copying every single variable into the new env makes
    // it very slow (?) to search for a variable because for every single parent
    // we encounter the same variables again...

    //if (env->parent) {
    hash_each(env->values, {
        //if (!hash_has(env->parent->values, strdup(key))) {
        hash_set(copy->values, strdup(key), lval_copy(val));
        //}
    });
    //}

    return copy;
}

lval* lenv_get(lenv* env, lval* name) {
    do {
        lval* value = hash_get(env->values, name->sym);
        if (value) { return lval_copy(value); }
    } while ((env = env->parent) != NULL);

    return lval_err("Unbound symbol: '%s'", name->sym);
}

void lenv_put(lenv* env, lval* name, lval* value) {
    hash_set(env->values, strdup(name->sym), lval_copy(value));
    assertf(hash_has(env->values, name->sym), "insert failed");
}

void lenv_def(lenv* env, lval* name, lval* value) {
    // Seek parent
    while (env->parent) {
        env = env->parent;
    }

    lenv_put(env, name, value);
}

char* lenv_func_name(lenv* env, lbuiltin func) {
    hash_each(env->values, {
        lval* value = val;
        if (value->type == LVAL_FUNC && value->builtin == func) {
            return strdup(key);
        }
    });

    return NULL;
}

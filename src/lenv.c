#include "lenv.h"


lenv* lenv_new(void) {
    lenv* env = xmalloc(sizeof(lenv));
    env->parent = NULL;
    env->values = hash_new();

    return env;
}

void lenv_del(lenv* env) {
    hash_each(env->values, {
        // Delete the value
        lval_del(val);
        // Free memory allocated in lenv_put by calling strdup
        xfree((char*) key);
    });
    hash_free(env->values);

    xfree(env);
}

lenv* lenv_copy(lenv* env) {
    lenv* copy = lenv_new();

    // FIXME: I suspect copying every single variable into the new env makes
    // it very slow (?) to search for a variable because for every single parent
    // we encounter the same variables again...
    // Do a good profiling on this to verify this hypothesis!

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
    // Keep in mind: The hash table stores a pointer to the key and
    // a pointer to the value.
    char* key;
    if (!hash_has(env->values, name->sym)) {
        // The hash table needs a pointer to the key. We can't just
        // use 'name->sym' because 'name' will propably be deleted
        // soon so we store a copy of it instead.
        key = strdup(name->sym);
    } else {
        // The key already exists. In this case we don't need to copy
        // the key because the stored on will remain untouched by 'hash_set'.
        key = name->sym;

        // If the key already exists, we have to remove the old value
        // manually because otherwise we loose the pointer to it and
        // we have a memory leak.
        lval* old_value = hash_get(env->values, key);
        lval_del(old_value);
    }

    hash_set(env->values, key, lval_copy(value));
    ASSERTF(hash_has(env->values, key), "insert failed");
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
            return (char*) key;
        }
    });

    return NULL;
}

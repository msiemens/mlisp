#include "utils.h"

#if defined (__WIN32__)
    char* strdup(const char * s) {
        size_t len = strlen(s) + 1;
        char* p = xmalloc(len);

        return p ? memcpy(p, s, len) : NULL;
    }
#endif

void* xmalloc(size_t n) {
    void* ptr = malloc(n);
    if (!ptr && n != 0) {
        die("Out of memory, malloc failed!\n");
    }

    return ptr;
}

void* xrealloc(void* ptr, size_t n) {
    ptr = realloc(ptr, n);
    if (!ptr && n != 0) {
        die("Out of memory, realloc failed!\n");
    }

    return ptr;
}

void die(char* msg) {
    fprintf(stderr, msg);
    fflush(stderr);
    abort();
}

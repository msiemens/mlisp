#include <stdarg.h>

#include "utils.h"


char* strdup(const char * s) {
    size_t len = strlen(s) + 1;
    char* p = xmalloc(len);

    return p ? memcpy(p, s, len) : NULL;
}

char* xsprintf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    size_t required_size = vsnprintf(NULL, 0, fmt, va);
    char* buffer = xmalloc(required_size + 1);
    vsnprintf(buffer, required_size + 1, fmt, va);

    va_end(va);

    return buffer;
}

char* strappend(char* dest, char* src, size_t size) {
    dest = xrealloc(dest, size);
    strcat(dest, src);
    return dest;
}

void xfree(void* ptr) {
    ASSERTF(ptr != NULL, "attempt to free a NULL ptr!");
    free(ptr);
    ptr = NULL;
}

void* xmalloc(size_t n) {
    void* ptr = malloc(n);
    if (!ptr && n != 0) {
        die("[FATAL ERROR] Out of memory, malloc failed!\n");
    }

    return ptr;
}

void* xrealloc(void* ptr, size_t n) {
    ptr = realloc(ptr, n);
    if (!ptr && n != 0) {
        die("[FATAL ERROR] Out of memory, realloc failed!\n");
    }

    return ptr;
}

void die(char* msg) {
    fprintf(stderr, msg);
    fflush(stderr);
    abort();
}

#include <float.h>
#include <math.h>

#include "utils.h"


bool fcmp(double f1, double f2) {
    return fabs(f1 - f2) < DBL_EPSILON;
}

char* strdup(const char * s) {
    size_t len = strlen(s) + 1;
    char* p = xmalloc(len);

    return p ? memcpy(p, s, len) : NULL;
}

char* strappend(char* dest, char* src, size_t size) {
    dest = xrealloc(dest, size);
    strcat(dest, src);
    return dest;
}

size_t xvsnprintf(char* s, size_t n, const char* format, va_list arg) {
    int written = vsnprintf(s, n, format, arg);
    if (written < 0) {
        ASSERTF(written > 0, "problems encoding fmt '%s'", format);
        return 0;  // Will never reach
    } else {
        return (size_t) written;
    }
}

char* xsprintf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    size_t required_size = xvsnprintf(NULL, 0, fmt, va);
    char* buffer = xmalloc(required_size + 1);
    xvsnprintf(buffer, required_size + 1, fmt, va);

    va_end(va);

    return buffer;
}

void xfree(void* ptr) {
    ASSERTF(ptr != NULL, "attempt to free a NULL ptr!");
    free(ptr);
}

void* xmalloc(size_t n) {
    void* ptr = malloc(n);

    if (!ptr && n != 0) {
        die("[FATAL ERROR] Out of memory, malloc failed!\n");
    }

#if defined DEBUG
    memset(ptr, 0xDE, n);
#endif


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

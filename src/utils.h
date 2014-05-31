#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

//! Prevent compiler from complaining about unused arguments
#define UNUSED(x) (void)(x)

// Assertions with custom messages
#define log_error(M, ...) __extension__({ \
    fprintf(stderr, "[ERROR] (%s:%d): " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
})

#define assertf(A, M, ...) __extension__({ \
    if(!(A)) { \
        log_error(M, ##__VA_ARGS__); \
        assert(A); \
    } \
})

#pragma GCC diagnostic pop

//! Free a pointer while checking for double free and then setting the ptr to NULL
#define FREE(ptr) assertf(ptr != NULL, "attempt to free a null ptr"); free(ptr); ptr = NULL;

//! Append a src string to a dest string. Need the new dest length.
#define STRAPPEND(dest, src, new_len) \
    dest = xrealloc(dest, new_len); \
    strcat(dest, src);

//! Allocate space for the src and then copy it
// \returns a pointer to src
#define MSTRCPY(src, dst) \
    xmalloc(strlen(src) + 1); \
    strcpy(dst, src);

//! Resize dst to contain src and then copy it
// \returns a pointer to src
#define RSTRCPY(src, dst) \
    xrealloc(dst, strlen(src) + 1); \
    strcpy(dst, src);

#if defined (__WIN32__)
    char* strdup(const char * s);
#endif


void* xmalloc(size_t n);

void* xrealloc(void* ptr, size_t n);

void die(char* msg);

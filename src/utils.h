#pragma once

#if !defined(MLISP_NOINCLUDE)
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
#endif


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// Don't use __extension__ on non-gnu compilers
#if (!defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER))
    #define __extension__(arg) arg
#endif

//! Prevent compiler from complaining about unused arguments
#define UNUSED(x) (void)(x)

// Assertions with custom messages
#define log_error(M, ...) __extension__({ \
    fprintf(stderr, "[ERROR] (%s:%d): " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
})

#if defined(NDEBUG)
    #define assertf(...)
#else
    #define assertf(A, ...) __extension__({ \
        if(!(A)) { \
            log_error(__VA_ARGS__); \
            assert(A); \
        } \
    })
#endif

#pragma GCC diagnostic pop


char* strdup(const char * s);

char* xsprintf(const char* fmt, ...);

//! Append a src string to a dest string. Need the new dest length
char* strappend(char* dest, char* src, size_t size);

//! Free a pointer while checking for double free and then setting the ptr to NULL
void xfree(void* ptr);

void* xmalloc(size_t n);

void* xrealloc(void* ptr, size_t n);

void die(char* msg);

#pragma once

// Don't use __extension__ on non-gnu compilers
#if (!defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER))
    #define __extension__(arg) arg
#endif

typedef float PRECISION;
typedef long  PRECISION_INT;

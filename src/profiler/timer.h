/**
 * \file    timer.h
 * \brief   A pretty precise timer.
 */
 
#if defined(_WIN32)
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

#include <stdbool.h>

typedef struct timer_inst {
    bool stopped; ///< Whether the timer has been stopped.
#if defined(_WIN32)
    LARGE_INTEGER frequency;    ///< The cpu frequency
    LARGE_INTEGER start_count;  ///< Number of cpu ticks at start of measuring
    LARGE_INTEGER end_count;    ///< Number of cpu ticks at end of measuring
#else
    timespec start_count;       ///< The time at start of measuring
    timespec end_count;         ///< The time at end of measuring
#endif
} timer_inst;

timer_inst* timer_init(void);
void timer_free(timer_inst* inst);
void timer_start(timer_inst* inst);
void timer_stop(timer_inst* inst);
double timer_get_elapsed(timer_inst* inst);

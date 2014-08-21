#include <stdlib.h>

#include "timer.h"

timer_inst* timer_init(void) {
    timer_inst* inst = malloc(sizeof(timer_inst));

    inst->stopped = 0;

#if defined(WIN32)
    QueryPerformanceFrequency(&inst->frequency);
    inst->start_count.QuadPart   = 0;
    inst->end_count.QuadPart     = 0;
#else
    inst->start_count.tv_sec     = 0;
    inst->start_count.tv_usec    = 0;
    inst->end_count.tv_sec       = 0;
    inst->end_count.tv_usec      = 0;
#endif

    return inst;
}

void timer_free(timer_inst* inst) {
    free(inst);
}

void timer_start(timer_inst* inst) {
    #if defined(WIN32)
        QueryPerformanceCounter(&inst->start_count);
    #else
        clock_gettime(CLOCK_MONOTONIC, &inst->start_count);
    #endif
}

void timer_stop(timer_inst* inst) {
    inst->stopped = 1;

    #if defined(WIN32)
        QueryPerformanceCounter(&inst->end_count);
    #else
        clock_gettime(CLOCK_MONOTONIC, &inst->end_count);
    #endif
}

double timer_get_elapsed(timer_inst* inst) {
    if (!inst->stopped) {
        timer_stop(inst);
    }

#if defined(WIN32)
    double ms_start_time = inst->start_count.QuadPart * (1e3 / inst->frequency.QuadPart);
    double ms_end_time   = inst->end_count.QuadPart   * (1e3 / inst->frequency.QuadPart);
#else
    double ms_start_time = inst->start_count.tv_sec   * 1e3 + inst->start_count.tv_usec * 1e-3;
    double ms_end_time   = inst->end_count.tv_sec     * 1e3 + inst->end_count.tv_usec   * 1e-3;
#endif

    return ms_end_time - ms_start_time;
}
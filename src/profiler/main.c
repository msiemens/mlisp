#include <stdio.h>
#include <windows.h>

#include "timer.h"
#include "mlisp.h"

#define RUNS 1000
#define WARMUP_RUNS 10
static double results[RUNS] = {0};

static mpc_parser_t* lispy;
static lenv* env;

void _parse(char* input) {
    lval* result = NULL;
    parse("<profiler>", input, env, &result, NULL);
}

void init(void) {
    parser_init();
    lispy = parser_get();
    env = lenv_new();
    builtins_init(env);

    _parse("(def {nil} {})");
    _parse("(def {true} 1)");
    _parse("(def {otherwise} true)");
    _parse(
        "(def {function} (lambda {args body} {\
            def (head args) (lambda (tail args) body)\
        }))"
    );
    _parse(
        "(function {unpack f xs} {\
            eval (join (list f) xs)\
        })"
    );
    _parse("(function {1st l} { eval (head l) })");
    _parse("(function {2nd l} { eval (head (tail l)) })");
    _parse(
        "(function {select ... cases} {\
            if (== cases nil)\
                {error \"No cases given!\"}\
                {if (1st (1st cases))\
                    {2nd (1st cases)}\
                    {unpack select (tail cases)} }\
        })"
    );
    _parse(
        "(function {fib n} {\
            select\
                { (== n 0) 0 }\
                { (== n 1) 1 }\
                { otherwise (+ (fib (- n 1)) (fib (- n 2))) }\
        })"
    );
}

int main(int argc, char** argv) {
    UNUSED(argv);

    init();

    int profile = 1;
    if (argc > 1) {
        profile = 0;
    }

    timer_inst* timer = NULL;
    if (profile) timer = timer_init();

    // Prepare command
    mpc_result_t r;
    mpc_parse("<profiler>", "fib 10", lispy, &r);
    lval* command = parse_tree(r.output);

    // Warmup run
    for (int i = 0; i < WARMUP_RUNS; i++) {
        lval* result = eval(env, lval_copy(command));
        lval_del(result);
    }

    // Do measurements
    for(int i = 0; i < RUNS; i++) {
        printf("\rRun %d/%d", (i + 1), RUNS);

        // Run command
        if (profile) timer_start(timer);
        lval* result = eval(env, lval_copy(command));
        if (profile) timer_stop(timer);

        // Cleanup
        lval_del(result);

        // Store result
        if (profile) results[i] = timer_get_elapsed(timer);
    }

    if (!profile) return 0;

    // Calculate avg and stddev
    double average = 0;
    double sum = 0;

    for (int i = 0; i < RUNS; i++) {
        sum += results[i];
    }
    average = sum / RUNS;

    double variance = 0;
    double tmp = 0;

    for (int i = 0; i < RUNS; i++) {
        tmp += (results[i] - average) * (results[i] - average);
    }
    variance = tmp / RUNS;

    double stddev = 0;
    stddev = sqrt(variance);

    // Print results
    //printf("%fms\n", timer_get_elapsed(timer));
    printf("%d runs\n", RUNS);
    printf("Avg: %fms\n", average);
    printf("StdDev: %fms\n", stddev);

    if (profile) timer_free(timer);
}

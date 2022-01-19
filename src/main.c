#include "include/core.h"
#include "include/op.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>

const int ERROR_MSG_LEVEL = 1;

#define exitIfError(_a, _c) { \
    Error r = _a; \
    if (r) {  \
        Error_print(r, _c, ERROR_MSG_LEVEL);  \
        exit(1);  \
    } \
}

static inline struct timespec now()
{
    struct timespec time;
    timespec_get(&time, TIME_UTC);
    return time;
}

static inline long diff(struct timespec* start, struct timespec* end)
{
    return end->tv_sec*1e9 + end->tv_nsec - start->tv_sec*1e9 - start->tv_nsec;
}

int main(int argc, char** argv){
    if (argc == 1){
        return 0;
    }

    char* fileName = argv[1];
    Mem m = Mem_new();
    Mem_nmem_push(&m, Value(Long, 0));
    Code c = Code_new();

    op_initOpTable();
    exitIfError(Code_from(&m, &c, Generator_File(fileName)), &c);
    exitIfError(Code_updateSymIdx(&m, &c), &c);

    struct timespec run_start = now();
    exitIfError(run(&m, &c), &c);
    struct timespec run_end = now();

    printf("run: %.3f\n", diff(&run_start, &run_end) / 1.0e9);

    return 0;
}

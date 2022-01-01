#include "include/core.h"
#include "include/op.h"
#include <stdio.h>
#include <limits.h>

const int ERROR_MSG_LEVEL = 1;

#define exitIfError(_a, _c) { \
    Error r = _a; \
    if (r) {  \
        Error_print(r, _c, ERROR_MSG_LEVEL);  \
        exit(1);  \
    } \
}

int main(int argc, char** argv){
    if (argc == 1){
        return 0;
    }

    char* fileName = argv[1];
    Mem m = Mem_new();
    Code c = Code_new();

    op_initOpTable();
    exitIfError(Code_fromFile(fileName, &m, &c), &c);
    exitIfError(run(&m, &c), &c);

    for (int i = 0; i < Mem_pmem_len(&m); i++){
        double d;
        Mem_mem_at(&m, i, &d);
        printf("%d ", (int)d);
    }

    return 0;
}

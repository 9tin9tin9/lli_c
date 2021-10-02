#ifndef _TEST_H_
#define _TEST_H_

#include <stdio.h>
#include <stdbool.h>

#define START_TEST() bool TOTAL_RESULT = true;

#define REQUIRE(bool_) { \
    bool b = (bool_);\
    TOTAL_RESULT = TOTAL_RESULT && b; \
    if (!b) { \
        printf("%s failed\n", #bool_); \
    } \
}

#define SUM_UP() { if (!TOTAL_RESULT) exit(1); }


#endif

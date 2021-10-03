#include "include/error.h"
#include "include/code.h"
#include <stdio.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

void
Error_print(Error r, void* _c, int level)
{
    // cast type
    Code* c = _c;

    switch (r) {

    }
}

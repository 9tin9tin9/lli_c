#include "include/opdef.h"

#include <stdio.h>

Error
nop(Vec v, Mem* m, Signal* s)
{
    *s = Signal(None, 0);
    return Ok;
}

#include "opdef/mem.c"

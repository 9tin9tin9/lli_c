#include "include/opdef.h"

Error
nop(const Vec* v, Mem* m, Signal* s)
{
    *s = Signal(None, 0);
    return Ok;
}

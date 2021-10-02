#include "include/op.h"


Error
nop(Vec v, Mem* m, Signal* s)
{
    printf("no op\n");
    *s = Signal(None, 0);
    return Ok;
}

Error
mov(Vec v, Mem* m, Signal* s)
{
    Tok* des = Vec_at(v, 0, Tok);
    Tok* src = Vec_at(v, 1, Tok);
    double val;
    Error r = Tok_getValue(*src, *m, &val);
    if (r) return r;
    r = Tok_writeValue(*des, m, val);
    return r;
}

Error
cpy(Vec v, Mem* m, Signal* s)
{

}

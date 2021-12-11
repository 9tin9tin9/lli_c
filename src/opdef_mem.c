#include "include/opdef.h"

Error
mov(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    Tok* des = Vec_at(v, 1, Tok);
    Tok* src = Vec_at(v, 2, Tok);
    double val;
    Error r = Tok_getValue(*src, *m, &val);
    if (r) return r;
    r = Tok_writeValue(*des, m, val);
    return r;
}

Error
cpy(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 3);
    long des;
    Error r = Tok_getLoc(*Vec_at(v, 1, Tok), m, &des);
    if (r) return r;

    long src;
    r = Tok_getLoc(*Vec_at(v, 2, Tok), m, &src);
    if (r) return r;

    size_t size;
    r = Tok_getUint(*Vec_at(v, 3, Tok), *m, &size);
    if (r) return r;

    double val;
    for (size_t i = 0; i < size; i++){
        r = Mem_mem_at(*m, src, &val);
        if (r) return r;

        r = Tok_writeValue(Tok(Idx, des), m, val);
        if (r) return r;
        
        idxIncr(&des, 1);
        idxIncr(&src, 1);
    }
    return Ok;
}

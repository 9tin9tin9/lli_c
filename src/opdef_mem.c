#include "include/opdef.h"

Error
mov(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    Tok* des = Vec_at(v, 0, Tok);
    Tok* src = Vec_at(v, 1, Tok);
    double val;
    Error r = Tok_getValue(*src, *m, &val);
    if (r) return r;
    r = Tok_writeValue(*des, m, val);
    *s = Signal(None, 0);
    return r;
}

Error
cpy(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 3);
    long des;
    Error r = Tok_getLoc(*Vec_at(v, 0, Tok), m, &des);
    if (r) return r;

    long src;
    r = Tok_getLoc(*Vec_at(v, 1, Tok), m, &src);
    if (r) return r;

    size_t size;
    r = Tok_getUint(*Vec_at(v, 2, Tok), *m, &size);
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
    *s = Signal(None, 0);
    return Ok;
}

Error
var(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    HashIdx var;
    long idx;
    Error r = Tok_getSym(*Vec_at(v, 0, Tok), &var);
    if (r) return r;
    r = Tok_getLoc(*Vec_at(v, 1, Tok), m, &idx);
    if (r) return r;
    Mem_var_set(m, var.idx, idx);
    *s = Signal(None, 0);
    return Ok;
}

#define mutVarIdx(v_, m_, a_) \
    argcGuard(v_, 2); \
    HashIdx var; \
    size_t incrVal; \
    Error r = Tok_getSym(*Vec_at(v_, 0, Tok), &var); \
    if (r) return r; \
    r = Tok_getUint(*Vec_at(v_, 0, Tok), *m_, &incrVal); \
    long varIdx; \
    r = Mem_var_find(*m_, var, &varIdx); \
    if (r) return r; \
    a_(&varIdx, incrVal); \
    Mem_var_set(m_, var.idx, varIdx); \
    *s = Signal(None, 0); \
    return Ok;

Error
incr(Vec v, Mem* m, Signal* s)
{
    mutVarIdx(v, m, idxIncr);
}

Error
decr(Vec v, Mem* m, Signal* s)
{
    mutVarIdx(v, m, idxDecr);
}

Error
allc(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    Tok* sizeTok = Vec_at(v, 0, Tok);
    size_t sizeS;
    Error r = Tok_getUint(*sizeTok, *m, &sizeS);
    if (r) return r;
    for (int i = 0; i < sizeS; i++){
        Mem_pmem_push(m, 0);
    }
    r = Mem_mem_set(m, 0, sizeS);
    *s = Signal(None, 0);
    return r;
}

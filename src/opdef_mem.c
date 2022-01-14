#include "include/opdef.h"

Error
mov(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    double val;
    try(Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, &val));
    try(Tok_writeValue(Vec_at_unsafe(v, 0, Tok), m, val));
    *s = Signal(None, 0);
    return Ok;
}

Error
cpy(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 3);
    long des;
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &des));
    long src;
    try(Tok_getLoc(Vec_at_unsafe(v, 1, Tok), m, &src));
    size_t size;
    try(Tok_getUint(Vec_at_unsafe(v, 2, Tok), m, &size));

    double val;
    for (size_t i = 0; i < size; i++){
        try(Mem_mem_at(m, src, &val));
        try(Tok_writeValue(&Tok(Idx, des), m, val));
        idxIncr(&des, 1);
        idxIncr(&src, 1);
    }
    *s = Signal(None, 0);
    return Ok;
}

Error
var(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    HashIdx var;
    long idx;
    try(Tok_getSym(Vec_at_unsafe(v, 0, Tok), &var));
    try(Tok_getLoc(Vec_at_unsafe(v, 1, Tok), m, &idx));
    Mem_var_set(m, var.idx, idx);
    *s = Signal(None, 0);
    return Ok;
}

Error
loc(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    long i;
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &i));
    Mem_mem_set(m, 0, i);
    *s = Signal(None, 0);
    return Ok;
}

Error
incr(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    long incrVal;
    Tok* v0 = Vec_at_unsafe(v, 0, Tok);
    try(Tok_getInt(Vec_at_unsafe(v, 1, Tok), m, &incrVal));
    if (v0->tokType != Var){
        return Error_WrongArgType;
    }
    long varIdx;
    try(Mem_var_find(m, &v0->Var, &varIdx));
    idxIncr(&varIdx, incrVal);
    Mem_var_set(m, v0->Var.idx, varIdx);
    *s = Signal(None, 0);
    return 0;
}

Error
allc(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    Tok* sizeTok = Vec_at_unsafe(v, 0, Tok);
    size_t sizeS;
    try(Tok_getUint(sizeTok, m, &sizeS));
    for (int i = 0; i < sizeS; i++){
        Mem_pmem_push(m, 0);
    }
    try(Mem_mem_set(m, 0, sizeS));
    *s = Signal(None, 0);
    return Ok;
}

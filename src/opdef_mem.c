#include "include/opdef.h"

Error
mov(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    Value val;
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

    Value val;
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
    Mem_mem_set(m, 0, Value('L', i));
    *s = Signal(None, 0);
    return Ok;
}

Error
allc(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    Tok* sizeTok = Vec_at_unsafe(v, 0, Tok);
    size_t sizeS;
    try(Tok_getUint(sizeTok, m, &sizeS));
    for (int i = 0; i < sizeS; i++){
        Mem_pmem_push(m, Value('L', 0));
    }
    *s = Signal(None, 0);
    return Ok;
}

Error
push(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    size_t ptr;
    Value val;
    try(Tok_getUint(Vec_at_unsafe(v, 0, Tok), m, &ptr));
    ptr++;
    try(Tok_writeValue(Vec_at_unsafe(v, 0, Tok), m, Value('L', ptr)));
    try(Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, &val));
    try(Mem_mem_set(m, ptr, val));
    *s = Signal(None, 0);
    return Ok;
}

Error
pop(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    size_t ptr;
    Value val;
    try(Tok_getUint(Vec_at_unsafe(v, 0, Tok), m, &ptr));
    try(Mem_mem_at(m, ptr, &val));
    Mem_mem_set(m, 0, val);
    ptr--;
    try(Tok_writeValue(Vec_at_unsafe(v, 0, Tok), m, Value('L', ptr)));
    *s = Signal(None, 0);
    return Ok;
}

#include "include/opdef.h"

Error
mov(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    Value val;
    try(Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, &val));
    try(Tok_writeValue(Vec_at_unsafe(v, 0, Tok), m, &val));
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
        try(Tok_writeValue(&Tok(Idx, des), m, &val));
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
    *Vec_at_unsafe(&m->mem, 0, Value) = Value(Long, i);
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
        Mem_mem_push(m, 1);
    }
    *s = Signal(None, 0);
    return Ok;
}

Error
push(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    long idx;
    Value* ptr;
    Value val;

    // getUint
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
    if (idx < 0) return Error_CannotWriteToNMem;
    ptr = Vec_at_unsafe(&m->mem, idx, Value);
    if (ptr->type != 'L') return Error_NotInteger;
    if (ptr->Long < 0) return Error_CannotWriteToNMem;

    // writeValue
    ptr->Long++;

    try(Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, &val));
    try(Mem_mem_set(m, ptr->Long, &val));
    *s = Signal(None, 0);
    return Ok;
}

Error
pop(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    long idx;
    Value* ptr;
    Value val;

    // getUint
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
    if (idx < 0) return Error_CannotWriteToNMem;
    ptr = Vec_at_unsafe(&m->mem, idx, Value);
    if (ptr->type != 'L') return Error_NotInteger;

    try(Mem_mem_at(m, ptr->Long, &val));
    *Vec_at_unsafe(&m->mem, 0, Value) = val;

    // writeValue
    ptr->Long--;
    *s = Signal(None, 0);
    return Ok;
}

Error
ltof(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    long idx;
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
    if (idx < 0) return Error_CannotWriteToNMem;
    Value* val = Vec_at_unsafe(&m->mem, idx, Value);
    if (val->type == 'L'){
        val->Double = val->Long;
    }
    return Ok;
}

Error
ftol(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    long idx;
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
    if (idx < 0) return Error_CannotWriteToNMem;
    Value* val = Vec_at_unsafe(&m->mem, idx, Value);
    if (val->type == 'D'){
        val->Long = val->Double;
    }
    return Ok;
}

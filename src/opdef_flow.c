#include "include/opdef.h"

Error
jmp(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    size_t loc;
    // try(Mem_label_find(m, &Vec_at_unsafe(v, 0, Tok)->Sym, &loc));
    try(Tok_getUint(Vec_at_unsafe(v, 0, Tok), m, &loc));
    *s = Signal(Jmp, loc);
    return Ok;
}

Error
jc(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    double cond;
    try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, &cond));
    if (cond){
        size_t loc;
        // try(Mem_label_find(m, &Vec_at_unsafe(v, 1, Tok)->Sym, &loc));
        try(Tok_getUint(Vec_at_unsafe(v, 1, Tok), m, &loc));
        *s = Signal(Jmp, loc);
    }else{
        *s = Signal(None, 0);
    }
    return Ok;
}

Error
lbl(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    HashIdx label;
    try(Tok_getSym(Vec_at_unsafe(v, 0, Tok), &label));
    *s = Signal(SetLbl, label.idx);
    return Ok;
}

Error
call(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    Tok* idx = Vec_at_unsafe(v, 0, Tok);
    size_t val;
    double curr;
    size_t loc;
    try(Tok_getUint(idx, m, &val));
    try(Tok_writeValue(idx, m, ++val));
    try(Mem_mem_at(m, -1, &curr));
    try(Mem_mem_set(m, val, curr+1));
    try(Tok_getUint(Vec_at_unsafe(v, 1, Tok), m, &loc));
    *s = Signal(Jmp, loc);
    return Ok;
}

Error
ret(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    Tok* idx = Vec_at_unsafe(v, 0, Tok);
    size_t val;
    double loc;
    try(Tok_getUint(idx, m, &val));
    try(Mem_mem_at(m, val, &loc));
    try(Tok_writeValue(idx, m, --val));
    *s = Signal(Jmp, loc);
    return Ok;
}

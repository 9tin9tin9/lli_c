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
curr(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 0);
    *s = Signal(Curr, 0);
    return Ok;
}

Error
res(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1)
    HashIdx label;
    size_t loc;
    try(Tok_getSym(Vec_at_unsafe(v, 0, Tok), &label));
    try(Mem_label_find(m, &label, &loc));
    Mem_mem_set(m, 0, (double)loc);
    *s = Signal(None, 0);
    return Ok;
}

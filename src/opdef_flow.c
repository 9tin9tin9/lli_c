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
    Value cond;
    try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, &cond));
    if (cond.Long){
        size_t loc;
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
    long idx;
    Value* ptr;
    Value curr;
    size_t loc;

    // getUint
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
    if (idx < 0) return Error_CannotWriteToNMem;
    ptr = Vec_at_unsafe(&m->pmem, idx, Value);
    if (ptr->type != 'L') return Error_NotInteger;
    if (ptr->Long < 0) return Error_CannotWriteToNMem;

    // writeValue
    ptr->Long++;

    try(Mem_mem_at(m, -1, &curr));
    curr.Long++;
    try(Mem_mem_set(m, ptr->Long, curr));
    try(Tok_getUint(Vec_at_unsafe(v, 1, Tok), m, &loc));
    *s = Signal(Jmp, loc);
    return Ok;
}

Error
ret(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    Value* ptr;
    Value loc;
    long idx;

    // getUint
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
    if (idx < 0) return Error_CannotWriteToNMem;
    ptr = Vec_at_unsafe(&m->pmem, idx, Value);
    if (ptr->type != 'L') return Error_NotInteger;

    try(Mem_mem_at(m, ptr->Long, &loc));

    // writeValue
    ptr->Long--;

    *s = Signal(Jmp, loc.Long);
    return Ok;
}

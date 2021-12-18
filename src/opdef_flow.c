#include "include/opdef.h"

Error
jmp(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    HashIdx label;
    size_t loc;
    Error r = Tok_getSym(*Vec_at(v, 0, Tok), &label);
    if (r) return r;
    r = Mem_label_find(*m, label, &loc);
    if (r) return r;
    *s = Signal(Jmp, loc);
    return Ok;
}

Error
jc(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    double cond;
    Error r = Tok_getValue(*Vec_at(v, 0, Tok), *m, &cond);
    if (r) return r;
    if (cond != 0){
        HashIdx label;
        size_t loc;
        r = Tok_getSym(*Vec_at(v, 1, Tok), &label);
        if (r) return r;
        r = Mem_label_find(*m, label, &loc);
        if (r) return r;
        *s = Signal(Jmp, loc);
    }else{
        *s = Signal(None, 0);
    }
    return Ok;
}

Error
lbl(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    HashIdx label;
    Error r = Tok_getSym(*Vec_at(v, 0, Tok), &label);
    if (r) return r;
    *s = Signal(SetLbl, label.idx);
    return Ok;
}

Error
als(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    HashIdx alias, label;
    size_t loc;
    Error r = Tok_getSym(*Vec_at(v, 0, Tok), &alias);
    if (r) return r;
    r = Tok_getSym(*Vec_at(v, 1, Tok), &label);
    if (r) return r;
    r = Mem_label_find(*m, label, &loc);
    if (r) return r;
    *s = Signal(SetAls, { alias.idx, loc });
    return Ok;
}

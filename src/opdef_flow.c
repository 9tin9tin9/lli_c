#include "include/opdef.h"

Error
jmp(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    HashIdx label;
    size_t loc;
    try(Tok_getSym(*Vec_at(v, 0, Tok), &label));
    try(Mem_label_find(*m, label, &loc));
    *s = Signal(Jmp, loc);
    return Ok;
}

Error
jc(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    double cond;
    try(Tok_getValue(*Vec_at(v, 0, Tok), *m, &cond));
    if (cond != 0){
        HashIdx label;
        size_t loc;
        try(Tok_getSym(*Vec_at(v, 1, Tok), &label));
        try(Mem_label_find(*m, label, &loc));
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
    try(Tok_getSym(*Vec_at(v, 0, Tok), &label));
    *s = Signal(SetLbl, label.idx);
    return Ok;
}

Error
als(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    HashIdx alias, label;
    size_t loc;
    try(Tok_getSym(*Vec_at(v, 0, Tok), &alias));
    try(Tok_getSym(*Vec_at(v, 1, Tok), &label));
    try(Mem_label_find(*m, label, &loc));
    *s = Signal(SetAls, { alias.idx, loc });
    return Ok;
}

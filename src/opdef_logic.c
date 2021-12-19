#include "include/opdef.h"

enum UnaryLogicType{
    AND, OR
};

Error
unaryLogic(enum UnaryLogicType t, Vec v, Mem* m, Signal* s){
    argcGuard(v, 2);
    double left, right;
    try(Tok_getValue(*Vec_at(v, 0, Tok), *m, &left));
    try(Tok_getValue(*Vec_at(v, 1, Tok), *m, &right));
    double result = t == AND?
        left != 0 && right != 0 :
        left != 0 || right != 0;
    Mem_mem_set(m, 0, result);
    *s = Signal(None, 0);
    return Ok;
}

Error
and(Vec v, Mem* m, Signal* s)
{
    return unaryLogic(AND, v, m, s);
}

Error
or(Vec v, Mem* m, Signal* s)
{
    return unaryLogic(OR, v, m, s);
}

Error
not(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    double value;
    try(Tok_getValue(*Vec_at(v, 0, Tok), *m, &value));
    double result = value == 0.0;
    Mem_mem_set(m, 0, result);
    *s = Signal(None, 0);
    return Ok;
}

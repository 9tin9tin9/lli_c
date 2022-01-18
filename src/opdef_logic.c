#include "include/opdef.h"

enum UnaryLogicType{
    AND, OR
};

Error
unaryLogic(enum UnaryLogicType t, const Vec* v, Mem* m, Signal* s){
    argcGuard(v, 2);
    Value left, right;
    try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, &left));
    try(Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, &right));
    long result = t == AND?
        left.Long != 0 && right.Long != 0:
        left.Long != 0 || right.Long != 0;
    *Vec_at_unsafe(&m->pmem, 0, Value) = Value(Long, result);
    *s = Signal(None, 0);
    return Ok;
}

Error
and(const Vec* v, Mem* m, Signal* s)
{
    return unaryLogic(AND, v, m, s);
}

Error
or(const Vec* v, Mem* m, Signal* s)
{
    return unaryLogic(OR, v, m, s);
}

Error
not(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    Value value;
    try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, &value));
    long result = value.Long == 0;
    *Vec_at_unsafe(&m->pmem, 0, Value) = Value(Long, result);
    *s = Signal(None, 0);
    return Ok;
}

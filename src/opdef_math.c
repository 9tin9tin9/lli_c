#include "include/opdef.h"

Error
math_parseArg(Vec v, Mem m, double* left, double* right)
{
    argcGuard(v, 2);
    try(Tok_getValue(*Vec_at(v, 0, Tok), m, left));
    try(Tok_getValue(*Vec_at(v, 1, Tok), m, right));
    return Ok;
}

#define math(op_, v_, m_, s_) \
    double left, right; \
    try(math_parseArg(v_, *m_, &left, &right)); \
    double result = left op_ right; \
    Mem_mem_set(m_, 0, result); \
    *s_ = Signal(None, 0); \
    return Ok;

Error
add(Vec v, Mem* m, Signal* s)
{
    math(+, v, m, s);
}

Error
sub(Vec v, Mem* m, Signal* s)
{
    math(-, v, m, s);
}

Error
mul(Vec v, Mem* m, Signal* s)
{
    math(*, v, m, s);
}

Error
div_(Vec v, Mem* m, Signal* s)
{
    math(/, v, m, s);
}

Error
mod(Vec v, Mem* m, Signal* s)
{
    double left, right;
    try(math_parseArg(v, *m, &left, &right));
    if (left != (int)left || right != (int)right)
        return Error_ModOperandNotInteger;
    double result = (int)left % (int)right;
    Mem_mem_set(m, 0, result);
    *s = Signal(None, 0);
    return Ok;
}

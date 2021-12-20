#include "include/opdef.h"

Error
cmp_parseArg(const Vec* v, Mem* m, double* left, double* right)
{
    argcGuard(v, 2);
    try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, left));
    return Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, right);
}

#define cmp(op_, v_, m_, s_) \
    double left, right; \
    try(cmp_parseArg(v_, m_, &left, &right)); \
    double result = left op_ right; \
    Mem_mem_set(m_, 0, result); \
    *s_ = Signal(None, 0); \
    return Ok;

Error
eq(const Vec* v, Mem* m, Signal* s)
{
    cmp(==, v, m, s);
}

Error
ne(const Vec* v, Mem* m, Signal* s)
{
    cmp(!=, v, m, s);
}

Error
gt(const Vec* v, Mem* m, Signal* s)
{
    cmp(>, v, m, s);
}

Error
lt(const Vec* v, Mem* m, Signal* s)
{
    cmp(<, v, m, s);
}

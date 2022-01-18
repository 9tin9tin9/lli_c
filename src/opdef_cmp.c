#include "include/opdef.h"

Error
cmp_parseArg(const Vec* v, Mem* m, Value* left, Value* right)
{
    argcGuard(v, 2);
    try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, left));
    return Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, right);
}

#define cmp(op_, v_, m_, s_) \
    Value left, right; \
    try(cmp_parseArg(v_, m_, &left, &right)); \
    long result = left.Long op_ right.Long; \
    *Vec_at_unsafe(&m_->pmem, 0, Value) = Value(Long, result); \
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

#define cmpf(op_, v_, m_, s_) \
    Value left, right; \
    try(cmp_parseArg(v_, m_, &left, &right)); \
    if (left.type == 'L') left.Double = left.Long; \
    if (right.type == 'L') right.Double = right.Long; \
    double result = left.Double op_ right.Double; \
    *Vec_at_unsafe(&m_->pmem, 0, Value) = Value(Double, result); \
    *s_ = Signal(None, 0); \
    return Ok;

Error
eqf(const Vec* v, Mem* m, Signal* s)
{
    cmpf(==, v, m, s);
}

Error
nef(const Vec* v, Mem* m, Signal* s)
{
    cmpf(!=, v, m, s);
}

Error
gtf(const Vec* v, Mem* m, Signal* s)
{
    cmpf(>, v, m, s);
}

Error
ltf(const Vec* v, Mem* m, Signal* s)
{
    cmpf(<, v, m, s);
}

#include "include/opdef.h"

Error
math_parseArg(const Vec* v, Mem* m, Value* left, Value* right)
{
    argcGuard(v, 2);
    try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, left));
    try(Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, right));
    return Ok;
}

#define math(op_, v_, m_, s_) \
    Value left, right; \
    try(math_parseArg(v_, m_, &left, &right)); \
    long result = left.Long op_ right.Long; \
    Mem_mem_set(m_, 0, Value('L', result)); \
    *s_ = Signal(None, 0); \
    return Ok;

Error
add(const Vec* v, Mem* m, Signal* s)
{
    math(+, v, m, s);
}

Error
sub(const Vec* v, Mem* m, Signal* s)
{
    math(-, v, m, s);
}

Error
mul(const Vec* v, Mem* m, Signal* s)
{
    math(*, v, m, s);
}

Error
div_(const Vec* v, Mem* m, Signal* s)
{
    math(/, v, m, s);
}

Error
mod(const Vec* v, Mem* m, Signal* s)
{
    Value left, right;
    try(math_parseArg(v, m, &left, &right));
    long result = left.Long % right.Long;
    Mem_mem_set(m, 0, Value('L', result));
    *s = Signal(None, 0);
    return Ok;
}

#define incrDecr(v, m, s, op) \
    argcGuard(v, 1); \
    Value val; \
    long loc; \
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &loc)); \
    try(Mem_mem_at(m, loc, &val)); \
    val.Long op; \
    try(Mem_mem_set(m, loc, val)); \
    *s = Signal(None, 0); \
    return Ok; \

Error
inc(const Vec* v, Mem* m, Signal* s)
{
    incrDecr(v, m, s, ++);
}

Error
dec(const Vec* v, Mem* m, Signal* s)
{
    incrDecr(v, m, s, --);
}

#include "include/opdef.h"

Error
src(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    HashIdx name;
    try(Tok_getSym(Vec_at(v, 0, Tok), &name));
    *s = Signal(Src, Str_copy(&name.sym));
    return Ok;
}

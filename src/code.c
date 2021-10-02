#include "include/code.h"

Code
Code_new()
{
    return (Code){
        Vec(Line),
        0,
    };
}

size_t
Code_push(Code* self, Vec toks, size_t opcode)
{
    Vec_push(&self->lines, ((Line){opcode, toks}));
    return Vec_count(self->lines);
}

Line*
Code_at(Code self, size_t idx)
{
    return Vec_at(self.lines, idx, Line);
}

Line*
Code_curr(Code self)
{
    return Vec_at(self.lines, self.ptr, Line);
}

size_t
Code_len(Code self)
{
    return Vec_count(self.lines);
}

size_t
Code_ptr(Code self)
{
    return self.ptr;
}

void
Code_ptr_set(Code* self, size_t ptr)
{
    self->ptr = ptr;
}

void
Code_ptr_incr(Code* self, size_t delta)
{
    self->ptr += delta;
}

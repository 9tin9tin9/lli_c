#ifndef _CODE_H_
#define _CODE_H_

#include "container/vec.h"

typedef struct{
    size_t opcode;
    Vec toks;
}Line;

typedef struct {
    Vec lines; // Vec<Line>
    size_t ptr;
}Code;

Code Code_new();

// Vec will be MOVED
size_t Code_push(Code*, Vec, size_t);

Line* Code_at(Code, size_t);
Line* Code_curr(Code);
size_t Code_len(Code);
size_t Code_ptr(Code);
void Code_ptr_set(Code*, size_t);
void Code_ptr_incr(Code*, size_t);

#endif

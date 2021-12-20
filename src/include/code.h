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

#define Code_new() ((Code){Vec(Line), 0})

// Vec will be MOVED
#define Code_push(c_, t_, o_) Vec_push(&(c_)->lines, ((Line){(o_), *(t_)}))

#define Code_at(c_, i_) Vec_at(&(c_)->lines, (i_), Line)
#define Code_curr(c_) Vec_at(&(c_)->lines, (c_)->ptr, Line)
#define Code_len(c_) Vec_count(&(c_)->lines)
#define Code_ptr(c_) ((c_)->ptr)

#define Code_ptr_set(c_, p_) ( (c_)->ptr = (p_) )
#define Code_ptr_incr(c_) ( (c_)->ptr++ )

#endif

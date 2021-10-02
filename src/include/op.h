#ifndef _OP_H_
#define _OP_H_

#include "container/str.h"
#include "container/hashmap.h"
#include "mem.h"
#include "code.h"
#include <stdlib.h>

typedef struct{
    enum Type {
        None,
        SetLbl,
        SetAls,
        Jmp,
        Src,
    }type;
    union{
        short None;
        size_t SetLbl;
        struct{
            size_t alias;
            size_t loc;
        }SetAls;
        size_t Jmp;
        Str Src;
    };
}Signal;
#define Signal(type_, content_) ((Signal){type_, .type_ = content_})
Error Signal_respond(Signal, Mem*, Code*, Hashmap);

typedef Error (*OpFunc)(Vec, Mem*, Signal*);
#define defOp(name) Error name(Vec, Mem*, Signal*)

void init_op_table();
Error exec(Mem*, Code, Signal*);

// Extend Tok
Error Tok_getValue(Tok, Mem, double*);
Error Tok_getUint(Tok, Mem, size_t*);
Error Tok_getLoc(Tok, Mem*, long*);
Error Tok_writeValue(Tok, Mem*, double);
Error Tok_createLtl(Tok, Mem*, long*);
Error Tok_getSym(Tok, HashIdx*);

#endif

#ifndef _OP_H_
#define _OP_H_

#include "container/str.h"
#include "container/hashmap.h"
#include "mem.h"
#include "code.h"
#include <stdlib.h>

typedef struct{
    enum Type {
        Signal_None,
        Signal_SetLbl,
        Signal_Curr,
        Signal_Jmp,
    }type;
    union{
        char None;
        size_t SetLbl;
        char Curr;
        size_t Jmp;
    };
}Signal;
#define Signal(type_, ...) ((Signal){Signal_##type_, .type_ = __VA_ARGS__})
Error Signal_respond(const Signal*, Mem*, Code*);

typedef Error (*OpFunc)(const Vec*, Mem*, Signal*);

void op_initOpTable();
Error op_exec(Mem*, const Code*, Signal*);

// Hashmap<const char*, size_t>
extern Hashmap opIdxTable;
// Vec<OpFunc>
extern Vec funcVec;

// Extend Tok
Error Tok_getValue(const Tok*, const Mem*, Value*);
Error Tok_getUint(const Tok*, const Mem*, size_t*);
Error Tok_getInt(const Tok*, const Mem*, long*);
Error Tok_getLoc(const Tok*, Mem*, long*);
Error Tok_writeValue(const Tok*, Mem*, Value);
Error Tok_createLtl(const Tok*, Mem*, long*);
Error Tok_getSym(const Tok*, HashIdx*);

#endif

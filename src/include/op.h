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

// Hashmap<const char*, size_t>
extern Hashmap opIdxTable;

// Extend Tok
Error Tok_getValueNum(const Tok*, const Mem*, Value*);
Error Tok_getValueVar(const Tok*, const Mem*, Value*);
Error Tok_getValueIdx(const Tok*, const Mem*, Value*);
Error Tok_getValueSym(const Tok*, const Mem*, Value*);
#define Tok_getValue(type, tok, m, v) Tok_getValue##type(tok, m, v)

Error Tok_getLocVar(const Tok*, Mem*, long*);
Error Tok_getLocIdx(const Tok*, Mem*, long*);
Error Tok_getLocLtl(const Tok*, Mem*, long*);
#define Tok_getLoc(type, tok, m, v) Tok_getLoc##type(tok, m, v)

Error Tok_writeValueVar(const Tok*, Mem*, Value*);
Error Tok_writeValueIdx(const Tok*, Mem*, Value*);
Error Tok_writeValueLtl(const Tok*, Mem*, Value*);
#define Tok_writeValue(type, tok, m, v) Tok_writeValue##type(tok, m, v)

Error Tok_createLtl(const Tok*, Mem*, size_t*);
Error Tok_getSym(const Tok*, HashIdx*);

#endif

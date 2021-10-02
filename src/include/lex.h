#ifndef _LEX_H_
#define _LEX_H_

#include <stdlib.h>
#include "container/vec.h"
#include "container/str.h"
#include "error.h"

typedef struct {
    Str sym;
    size_t idx;
}HashIdx;

// sym will be MOVED to HashIdx
// make a copy before passing to this function if sym will be used again
HashIdx 
HashIdx_new(Str sym, size_t idx);

void 
HashIdx_del(HashIdx* hi);

typedef struct {
    enum Tok_Type {
        Num, Idx, Var, 
        VarIdx, Ltl, Sym, 
        Eof
    }tokType;
    union {
        double Num;
        long Idx;
        HashIdx Var;
        HashIdx VarIdx;
        Str Ltl;
        HashIdx Sym;
        char Eof;
    };
}Tok;

#define Tok(type_, content_) ((Tok){ .tokType = type_, .type_ = content_ })
int Tok_eq(Tok, Tok);
int HashIdx_eq(HashIdx left, HashIdx right);
// Str will be MOVED
int Tok_fromStr(Tok*, Str);
Error Tok_tokenize(Vec* des, Str s);

#endif

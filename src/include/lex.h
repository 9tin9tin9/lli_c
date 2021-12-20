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
HashIdx_new(const Str* sym, size_t idx);

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
int Tok_eq(const Tok*, const Tok*);
int HashIdx_eq(const HashIdx* left, const HashIdx* right);
// Str will be MOVED
Error Tok_fromStr(Tok*, Str*);
// Str: read only
Error lex_tokenize(Vec* des, const Str* s);

#endif

#ifndef _LEX_H_
#define _LEX_H_

#include <stdlib.h>
#include "container/vec.h"
#include "container/str.h"
#include "error.h"

typedef struct {
    enum {
        Double = 'D', Long = 'L'
    }type;
    union{
        double Double;
        long Long;
    };
}Value;

#define Value(t_, v_) (Value){.type = #t_[0], .t_ = v_}
static inline int Value_eq(const Value* l, const Value* r)
{
    return l->type == r->type && l->Double == r->Double;
}
long* Value_getL(Value* v);
double* Value_getD(Value* v);

typedef struct {
    Str sym;
    size_t idx;
}HashIdx;

struct Idx{
    enum Idx_Type{
        Idx_Type_Num, Idx_Type_Idx, Idx_Type_Var
    }type;
    union {
        long Num;
        struct Idx* Idx;
        HashIdx Var;
    };
};

// sym will be MOVED to HashIdx
// make a copy before passing to this function if sym will be used again
HashIdx 
HashIdx_new(const Str* sym, size_t idx);

typedef struct {
    enum Tok_Type {
        Num, Idx, Var, 
        Ltl, Sym, Eof
    }tokType;
    union {
        Value Num;
        struct Idx Idx;
        HashIdx Var;
        HashIdx Ltl;
        HashIdx Sym;
        char Eof;
    };
}Tok;

#define Tok(type_, ...) ((Tok){ .tokType = type_, .type_ = __VA_ARGS__ })
int Tok_eq(const Tok*, const Tok*);
int HashIdx_eq(const HashIdx* left, const HashIdx* right);
// Str will be MOVED
Error Tok_fromStr(Tok*, Str*);
// Str: read only
Error lex_tokenize(Vec* des, const Str* s);

#endif

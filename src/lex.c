#include "include/lex.h"
#include "include/error.h"

HashIdx
HashIdx_new(const Str* sym, size_t idx)
{
    return (HashIdx){
        *sym, idx
    };
}

int
HashIdx_eq(const HashIdx* left, const HashIdx* right)
{
    return strcmp(
        Str_at(&left->sym, 0),
        Str_at(&right->sym, 0)
        ) == 0 &&
    left->idx == right->idx;
}

int
Tok_eq(const Tok* left, const Tok* right)
{
    int eq = left->tokType == right->tokType;
    const struct Idx* leftIdx, *rightIdx;
    if (eq == 0) return 0;
    switch (left->tokType){
        case Num:
            return left->Num == right->Num;
        case Idx:
            leftIdx = &left->Idx;
            rightIdx = &right->Idx;
            while(1){
                if (leftIdx->type != rightIdx->type) return 0;
                switch (leftIdx->type){
                    case Idx_Type_Num:
                        if (leftIdx->Num == rightIdx->Num) return 1;
                    case Idx_Type_Var:
                        return HashIdx_eq(&leftIdx->Var, &rightIdx->Var);
                    case Idx_Type_Idx:
                        leftIdx = leftIdx->Idx;
                        rightIdx = rightIdx->Idx;
                }
                if (!(leftIdx && rightIdx)) return 0;
            }
        case Var:
            return HashIdx_eq(&left->Var, &right->Var);
        case Ltl:
            return strcmp(
                    Str_at(&left->Ltl, 0),
                    Str_at(&right->Ltl, 0)
                    ) == 0;
        case Sym:
            return HashIdx_eq(&left->Sym, &right->Sym);
        case Eof:
            return 1;
    }
}

#define case0to9 \
    case'0':case'1':case'2':case'3':case'4':\
    case'5':case'6':case'7':case'8':case'9':

// end_: where null pointer places
void shrinkStr(Str* s, size_t start, size_t end)
{
    *Str_at(s, end) = '\0';
    size_t newSize = end - start;
    char* new = malloc(newSize+1);
    memcpy(new, Str_at(s, start), newSize+1);
    free(s->array);
    s->array = new;
    s->size = newSize;
}

// end: the last non-null character
Error
eatIdx(struct Idx* idx, Str* s, size_t start, size_t end)
{
    if (*Str_at(s, end) != ']')
        return Error_UnterminatedIdx;
    else if (end-start == 1)
        return Error_EmptyIdx;
    
    switch (*Str_at(s, start+1)){
        // Var
        case '$':
            if (end-start == 2)
                return Error_MissingVarName;
            
            shrinkStr(s, start+2, end);
            *idx = (struct Idx){
                .type = Idx_Type_Var,
                .Var = HashIdx_new(s, 0)
            };
            return Ok;
        // Idx
        case '[':
            *idx = (struct Idx){
                .type = Idx_Type_Idx,
                .Idx = malloc(sizeof(struct Idx)),
            };
            return eatIdx(idx->Idx, s, start+1, end-1);
        // Num
        default:
            shrinkStr(s, start+1, end);
            char* ptr;
            long num = strtol(Str_at(s, 0), &ptr, 10);
            if (ptr == Str_at(s, 0))
                return Error_ParseIdxError;
            *idx = (struct Idx){
                .type = Idx_Type_Num,
                .Num = num,
            };
            return Ok;
    }
}

Error
Tok_fromStr(Tok* tok, Str* s)
{
    if (Str_len(s) == 0){
        *tok = (Tok){ Eof };
        return Ok;
    }

    switch (*Str_at(s, 0)){
        // Num
        case '-':
        case0to9;
            char* ptr;
            double num = strtod(Str_at(s, 0), &ptr);
            if (ptr == Str_at(s, 0)){
                return Error_ParseNumError;
            }
            *tok = (Tok){ Num, .Num = num };
            return Ok;

        // Idx
        case '[':
            *tok = Tok(Idx, {});
            return eatIdx(&tok->Idx, s, 0, s->size-2);

        // Var
        case '$':
            shrinkStr(s, 1, s->size-1);
            *tok = Tok(Var, HashIdx_new(s, 0));
            return Ok;

        // Ltl
        case '"':
            shrinkStr(s, 1, s->size-2);
            *tok = Tok(Ltl, *s);
            return Ok;

        // Sym
        default:
            *tok = Tok(Sym, HashIdx_new(s, 0));
            return Ok;
    }
}

#undef shrinkStr

Error
eat_token(
    Tok* tok, const Str* s, size_t* ptr, 
    char delim, char unexpect, int errno
    )
{
    enum State{
        WAITING = 'W',
        STRLTL = 'L',
        SYM = 'S',
        ENDED = 'E',
    };
    enum State state = WAITING;
    Str current = Str();
    int escaped = 0;
    size_t len = 0;

    while(1){
        char* c = Str_at(s, (*ptr)++);
        if (c == NULL || *c == 0){
            break;
        }
        if (*c == '#' && state != STRLTL){
            *ptr = Str_count(s);
            break;
        }

        if ((*c == ' ' || *c == '\t') && state != STRLTL){
            if (state != STRLTL){
                if (state != WAITING)
                    state = ENDED;
                continue;
            }

        }else if (*c == delim){
            if (state == WAITING){
                return Error_EmptyToken;
            }else if (state == SYM || state == ENDED){
                break;
            }

        }else if (*c == unexpect){
            if (state != STRLTL) {
                return Error_UnexpectedDelim;
            }

        }else if (state == ENDED){
            return Error_NonDelimAfterSymEnd;

        }else if (*c == '"'){
            if (state == WAITING){
                state = STRLTL;
            }else if (state == STRLTL){
                if (escaped == 0){
                    state = ENDED;
                }
            }else
                return Error_DoubleQuoteInMiddle;

        }else if (*c == '\\'){
            if (state == STRLTL){
                escaped = !escaped;
                if (escaped == 0){
                    Str_push(&current, *c);
                }
                continue;
            }

        }else if (state == WAITING){
            state = SYM;

        }else if (escaped){
            switch (*c) {
                case 'n':
                    *c = '\n';
                    break;
                case 't':
                    *c = '\t';
                    break;
                default:
                    return Error_UnknownEscapeSequence;
            }
        }
        escaped = 0;
        Str_push(&current, *c);
    }

           // `current` moved to Tok_fromStr
    return Tok_fromStr(tok, &current);
}

Error
eat_operator(Tok* tok, size_t* ptr, const Str* s)
{
    try(eat_token(tok, s, ptr, ':', ',', 0));
    if (tok->tokType != Sym && tok->tokType != Eof) 
        return Error_WrongTokTypeForOp;
    return Ok;
}

Error
eat_args(Tok* tok, size_t* ptr, const Str* s)
{
    return eat_token(tok, s, ptr, ',', ':', 0);
}

Error
lex_tokenize(Vec* des, const Str* s)
{
    // points to the idx that have read
    size_t ptr = 0;
    Tok tok;
    try(eat_operator(&tok, &ptr, s));
    switch (tok.tokType) {
        case Sym:
            Vec_push(des, tok);
            break;

        case Eof:
            return Ok;

        default:
            // Expects symbol as operator
            return Error_WrongTokTypeForOp;
    }
    while(1){
        try(eat_args(&tok, &ptr, s));
        if (tok.tokType == Eof){
            return 0;
        }
        Vec_push(des, tok);
    }
}

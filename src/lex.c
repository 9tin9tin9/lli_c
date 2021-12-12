#include "include/lex.h"
#include "include/error.h"

HashIdx
HashIdx_new(Str sym, size_t idx)
{
    return (HashIdx){
        sym, idx
    };
}

void
HashIdx_del(HashIdx* hi)
{
    Str_del(&hi->sym);
}

int
HashIdx_eq(HashIdx left, HashIdx right)
{
    return strcmp(
        Str_at(left.sym, 0),
        Str_at(right.sym, 0)
        ) == 0 &&
    left.idx == right.idx;
}

int
Tok_eq(Tok left, Tok right)
{
    int eq = left.tokType == right.tokType;
    if (eq == 0) return 0;
    switch (left.tokType){
        case Num:
            return left.Num == right.Num;
        case Idx:
            return left.Idx == right.Idx;
        case Var:
            return HashIdx_eq(left.Var, right.Var);
        case VarIdx:
            return HashIdx_eq(left.VarIdx, right.VarIdx);
        case Ltl:
            return strcmp(
                    Str_at(left.Ltl, 0),
                    Str_at(right.Ltl, 0)
                    ) == 0;
        case Sym:
            return HashIdx_eq(left.Sym, right.Sym);
        case Eof:
            return 1;
    }
}

#define case0to9 \
    case'0':case'1':case'2':case'3':case'4':\
    case'5':case'6':case'7':case'8':case'9':

// end_: where null pointer places
#define shrinkStr(s_, start_, end_) \
{ \
    *Str_at(s, end_) = '\0'; \
    size_t newSize = end_ - start_; \
    char* new = malloc(newSize+1); \
    memcpy(new, Str_at(s_, start_), newSize+1); \
    free(s_.array); \
    s_.array = new; \
    s.size = newSize; \
}

Error
Tok_fromStr(Tok* tok, Str s)
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

        // Idx | VarIdx
        case '[':
            if (Str_back(s) != ']'){
                return Error_UnterminatedIdx;
            }else if (Str_len(s) == 2){
                return Error_EmptyIdx;
            }
            if (*Str_at(s, 1) == '$'){
                // VarIdx
                if (Str_len(s) == 3){
                    return Error_MissingVarName;
                }
                shrinkStr(s, 2, s.size-2);
                *tok = Tok(VarIdx, HashIdx_new(s, 0));
                return Ok;
            }else{
                // Idx
                shrinkStr(s, 1, s.size-2);
                char* ptr;
                long idx = strtol(Str_at(s, 0), &ptr, 10);
                if (ptr == Str_at(s, 0)){
                    return Error_ParseIdxError;
                }
                *tok = Tok(Idx, idx);
                return Ok;
            }

        // Var
        case '$':
            shrinkStr(s, 1, s.size-1);
            *tok = Tok(Var, HashIdx_new(s, 0));
            return Ok;

        // Ltl
        case '"':
            shrinkStr(s, 1, s.size-2);
            *tok = Tok(Ltl, s);
            return Ok;

        // Sym
        default:
            *tok = Tok(Sym, s);
            return Ok;
    }
}

#undef shrinkStr

int
eat_token(
    Tok* tok, Str s, size_t* ptr, 
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
            break;

        }else if (*c == ' ' || *c == '\t'){
            if (state == SYM){
                state = ENDED;
                continue;
            }else if (state != STRLTL){
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
    return Tok_fromStr(tok, current);
}

Error
eat_operator(Tok* tok, size_t* ptr, Str s)
{
    Error r = eat_token(tok, s, ptr, ':', ',', 0);
    if (r) return r;
    if (tok->tokType != Sym && tok->tokType != Eof) 
        return Error_WrongTokTypeForOp;
    return Ok;
}

Error
eat_args(Tok* tok, size_t* ptr, Str s)
{
    return eat_token(tok, s, ptr, ',', ':', 0);
}

Error
lex_tokenize(Vec* des, Str s)
{
    // points to the idx that have read
    size_t ptr = 0;
    Tok tok;
    Error result = eat_operator(&tok, &ptr, s);
    // if error
    if (result) return result;
    switch (tok.tokType) {
        case Sym:
            Vec_push(des, tok);
            break;

        case Eof:
            return Ok;

        default:
            // Expects symbol as operator
            // set errno later
            return 1;
    }
    while(1){
        int result = eat_args(&tok, &ptr, s);
        // if error
        if (result) return result;
        if (tok.tokType == Eof){
            return 0;
        }
        Vec_push(des, tok);
    }
}

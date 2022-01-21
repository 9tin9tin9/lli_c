#include "include/core.h"
#include "include/op.h"
#include "include/opdef.h"
#include <stdio.h>
#include <limits.h>

// find symbol and then assign index or update table
#define _findAndUpdate(location, action) \
    tok = Vec_at(toks, 0, Tok);  \
    if (tok->tokType != Sym) \
        return Error_WrongArgType;  \
    idx = Hashmap_at(  \
            m->location,  \
            Str_raw(&tok->Sym.sym),  \
            size_t);  \
    if (!idx) {  \
        tok->Sym.idx = action;  \
        Hashmap_insert(  \
                &m->location,  \
                Str_raw(&tok->Sym.sym),  \
                tok->Sym.idx);  \
    }else{  \
        return Error_DuplicatedSymbols;  \
    }  \

Error
prerun(size_t opcode, Mem* m, Code* c, Vec* toks)
{
    Tok* tok;
    size_t* idx;
    Str name;
    switch (opcode){
        case OPCODE_LBL:
            _findAndUpdate(labelLookUp, Mem_label_add(m, Code_len(c)));
            break;
        
        case OPCODE_VAR:
            _findAndUpdate(varLookUp, Mem_var_add(m, 0));
            break;

        case OPCODE_SRC:
            name = Vec_at(toks, 0, Tok)->Sym.sym;
            try(Code_from(m, c, Generator_File(name.array)));
            break;
    }
    return Ok;
}

#undef _findAndUpdate

Error
preprocess(Mem* m, Code* c, Vec* toks)
{
    if (Vec_count(toks) == 0){
        return Ok;
    }

    // get opcode
    Tok* opTok = Vec_at(toks, 0, Tok);
    // guaranteed to be tokType == Sym
    size_t* opcode = Hashmap_at(
            opIdxTable, 
            opTok->Sym.sym.array,
            size_t
            );
    if (!opcode){
        return Error_UnknownOp;
    }
    opTok->Sym.idx = *opcode;
    // trunc first tok (op)
    toks->size--;
    memmove(toks->array,
            toks->array+toks->elem_size,
            toks->size*toks->elem_size);

    // checkArgc()
    try(prerun(*opcode, m, c, toks));
    Code_push(c, toks, *opcode);
    return Ok;
}

#define _updateLabel(_i)  \
    {  \
        Tok* t = Vec_at(&line->toks, _i, Tok);  \
        if (!t) return Error_WrongArgCount;  \
        if (t->tokType != Sym) continue;  \
        hi = &t->Sym;  \
    }  \
    hi = &Vec_at(&line->toks, _i, Tok)->Sym;  \
    idx = Hashmap_at(  \
            m->labelLookUp,  \
            Str_raw(&hi->sym),  \
            size_t);  \
    if (!idx)  \
        return Error_UndefinedLabel;  \
    hi->idx = *idx;  \
    break;

// loop throguh all lines to update HashIdx.idx
Error
resolveSym(Mem* m, Code* c)
{
    const size_t codeLen = Code_len(c);
    size_t* idx;
    Code_ptr_set(c, 0);
    for (int i = 0; i < codeLen; i++, Code_ptr_incr(c)){
        Line* line = Code_at(c, i);
        HashIdx* hi = &Vec_at(&line->toks, 0, Tok)->Sym;

        switch (line->opcode) {
            case OPCODE_JMP:
                _updateLabel(0);

            case OPCODE_JC:
                _updateLabel(1);

            case OPCODE_CALL:
                _updateLabel(1);
        }

        for (int j = 0; j < Vec_count(&line->toks); j++){
            hi = NULL;
            Tok* tok = Vec_at(&line->toks, j, Tok);
            // replace Var. Var maybe inside nested Idx
            if (tok->tokType == Var || tok->tokType == Idx){
                if (tok->tokType == Idx){
                    struct Idx* idx = &tok->Idx;
                    while(idx->type == Idx_Type_Idx) idx = idx->Idx;
                    if (idx->type == Idx_Type_Num) continue;
                    hi = &idx->Var;
                }else{
                    hi = &Vec_at(&line->toks, j, Tok)->Var;
                }
                idx = Hashmap_at(
                        m->varLookUp, 
                        Str_raw(&hi->sym), 
                        size_t);
                if (!idx)
                    return Error_UndefinedVar;
                hi->idx = *idx;
            }else
            // create string literals during preprocessing
            if (tok->tokType == Ltl){
                try(Tok_createLtl(tok, m, &tok->Sym.idx));
                tok->Sym.idx = -tok->Sym.idx;
            }
        }
    }
    Code_ptr_set(c, 0);
    return Ok;
}

#undef _updateLabel

Error
generator_File(void* _state, char** str)
{
    struct State {
        Generator g;
        void* file;
        int status;
    }* state = _state;
    static char line[MAX_INPUT] = {0};

    switch (state->status){
        case 0:
            state->file = fopen(state->file, "r");
            if (!state->file){
                return Error_CannotOpenFile;
            }
            state->status = 1;

        default:
            if (!fgets(line, MAX_INPUT, state->file)){
                if (ferror(state->file)){
                    return Error_CannotReadFile;
                }
                fclose(state->file);
                *str = NULL;
                state->status = 0;
            }else{
                // remove trailing \n
                const size_t len = strlen(line);
                if (len > 0 && line[len-1] == '\n')
                    line[len-1] = 0;
                *str = line;
            }
            return Ok;
    }
}

Error
generator_StrArr(void* _state, char** str)
{
    struct State {
        Generator g;
        void* str;
        int i;
    }* state = _state;
    static size_t i = 0;
    *str = ((char**)state->str)[i++];
    return Ok;
}

Error
Code_from(Mem* m, Code* c, void* _state)
{
    struct State {
        Generator g;
        void* str;
    }* state = _state;
    char* line;
    size_t linelen;
    Error r;

    while(1){
        Vec toks = Vec(Tok);
        try(state->g(state, &line));
        if (!line) break;
        Str wrapper = (Str){.size = strlen(line), .array = line};
        try(lex_tokenize(&toks, &wrapper));

        try(preprocess(m, c, &toks));
    }
    Code_push(c, &Vec(Tok), OPCODE_HALT);

    return Ok;
}

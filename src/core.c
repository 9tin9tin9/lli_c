#include "include/core.h"
#include "include/op.h"
#include "include/opdef.h"
#include <stdio.h>
#include <limits.h>

// find symbol and then assign index or update table
#define _findAndUpdate(location, action) \
    tok = Vec_at(toks, 1, Tok);  \
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
        tok->Sym.idx = *idx;  \
    }  \
    return Ok;  \

Error
createSymTable(size_t opcode, Mem* m, Code* c, Vec* toks)
{
    Tok* tok;
    size_t* idx;
    switch (opcode){
        case OPCODE_LBL:
            _findAndUpdate(labelLookUp, Mem_label_add(m, Code_len(c)+1));
        
        case OPCODE_VAR:
            _findAndUpdate(varLookUp, Mem_var_add(m, 0));
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

    try(createSymTable(*opcode, m, c, toks));
    Code_push(c, toks, *opcode);
    return Ok;
}

#define _updateLabel(_i)  \
    {  \
        Tok* t = Vec_at(&line->toks, _i, Tok);  \
        if (!t) return Error_WrongArgCount;  \
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
updateSymIdx(Mem* m, Code* c)
{
    const size_t codeLen = Code_len(c);
    size_t* idx;

    for (int i = 0; i < codeLen; i++){
        Line* line = Code_at(c, i);
        HashIdx* hi = &Vec_at(&line->toks, 0, Tok)->Sym;

        switch (hi->idx) {
            case OPCODE_JMP:
                _updateLabel(1);

            case OPCODE_JC:
                _updateLabel(2);
        }

        // replace Var. Var maybe inside nested Idx
            for (int j = 1; j < Vec_count(&line->toks); j++){
                hi = NULL;
                Tok* tok = Vec_at(&line->toks, j, Tok);
                if (tok->tokType != Var) continue; 

                if (tok->tokType == Idx){
                    struct Idx* idx = &tok->Idx;
                    while(idx->type == Idx_Type_Idx) idx = idx->Idx;
                    if (idx->type == Idx_Type_Num) continue;
                    hi = &idx->Var;
                }

                if (!hi) hi = &Vec_at(&line->toks, j, Tok)->Var;
                idx = Hashmap_at(
                        m->varLookUp, 
                        Str_raw(&hi->sym), 
                        size_t);
                if (!idx)
                    return Error_UndefinedVar;
                hi->idx = *idx;
            }
    }
    return Ok;
}

#undef _updateLabel

void
argSlice(Code* c, size_t from)
{
    for (int i = from; i < Code_len(c); i++){
        Line* l = Code_at(c, i);
        Vec t = l->toks;
        t.array += t.elem_size;
        t.size--;
        Vec a = Vec_copy(&t);
        Vec_del(&l->toks);
        l->toks = a;
    }
}

Error
Code_fromFile(const char* fileName, Mem* m, Code* c)
{
    size_t oldlen = Code_len(c);

    FILE* fileptr = fopen(fileName, "r");
    if (!fileptr){
        return Error_CannotOpenFile;
    }

    char line[MAX_INPUT] = {0};
    size_t linelen;
    Error r;

    while(fgets(line, MAX_INPUT, fileptr)){
        Vec toks = Vec(Tok);
        // DON'T free
        // remove trailing \n
        line[strlen(line)-1] = 0;
        Str wrapper = (Str){strlen(line), line};
        try(lex_tokenize(&toks, &wrapper));

        try(preprocess(m, c, &toks));
    }

    if (ferror(fileptr)) {
        return Error_CannotReadFile;
    }
    try(updateSymIdx(m, c));
    // replace toks with args only. Op sym is not needed anymore
    argSlice(c, oldlen);
    return Ok;
}

Error
run(Mem* m, Code* c)
{
    Signal s;
    while(Code_ptr(c) < Code_len(c)){
        try(op_exec(m, c, &s));
        try(Signal_respond(&s, m, c));
    }
    return Ok;
}

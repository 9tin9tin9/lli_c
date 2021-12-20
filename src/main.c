#include "include/op.h"
#include <stdio.h>
#include <limits.h>

const int ERROR_MSG_LEVEL = 1;

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
        // lbl | alias
        case 21:
        case 22:
            _findAndUpdate(labelLookUp, Mem_label_add(m, Code_len(c)));
        
        // var
        case 3:
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

    for (int i = 0; i < codeLen; i++, Code_ptr_incr(c)){
        Line* line = Code_at(c, i);
        HashIdx* hi = &Vec_at(&line->toks, 0, Tok)->Sym;

        switch (hi->idx) {
            // var | lbl
            case 3:
            case 21:
                break;

            // jmp
            case 19:
                _updateLabel(1);

            // jc | als
            case 20:
            case 22:
                _updateLabel(2);

            // replace Var or VarIdx
            default:
                // Var and VarIdx has same mem layout in union
                for (int j = 1; j < Vec_count(&line->toks); j++){
                    enum Tok_Type tt = Vec_at(&line->toks, j, Tok)->tokType;
                    if (tt != Var && tt != VarIdx){
                        continue;
                    }

                    hi = &Vec_at(&line->toks, j, Tok)->Var;
                    idx = Hashmap_at(
                            m->varLookUp, 
                            Str_raw(&hi->sym), 
                            size_t);
                    if (!idx)
                        return Error_UndefinedVar;
                    hi->idx = *idx;
                }
                break;
        }
    }
    Code_ptr_set(c, 0);
    return Ok;
}

#undef _updateLabel

void
argSlice(Code* c)
{
    for (int i = 0; i < Code_len(c); i++){
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
readFromFile(const char* fileName, Mem* m, Code* c)
{
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
        Code_ptr_incr(c);
    }

    if (ferror(fileptr)) {
        return Error_CannotReadFile;
    }
    Code_ptr_set(c, 0);
    try(updateSymIdx(m, c));
    // replace toks with args only. Op sym is not needed anymore
    argSlice(c);
    return Ok;
}

#define exitIfError(_c)  \
    if (r) {  \
        Error_print(r, _c, ERROR_MSG_LEVEL);  \
        exit(1);  \
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

int main(int argc, char** argv){
    if (argc == 1){
        return 0;
    }

    char* fileName = argv[1];
    Mem m = Mem_new();
    Code c = Code_new();

    op_initOpTable();
    Error r = readFromFile(fileName, &m, &c);
    exitIfError(&c);
    r = run(&m, &c);
    exitIfError(&c);

    return 0;
}

#include "include/op.h"
#include "include/opdef.h"
#include "include/core.h"
#include <stdio.h>
#include <math.h>

#define addEntry(op) { \
    Vec_push(&funcVec, &op); \
    Hashmap_insert(&opIdxTable, #op, Vec_count(&funcVec)-1); \
}

#define addEntryAlternate(op) { \
    Vec_push(&funcVec, &op##_); \
    Hashmap_insert(&opIdxTable, #op, Vec_count(&funcVec)-1); \
}

Hashmap opIdxTable = Hashmap();
Vec funcVec = Vec(OpFunc);

Error
print_num(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    size_t fd;
    double value;
    try(Tok_getUint(Vec_at(v, 0, Tok), m, &fd));
    try(Tok_getValue(Vec_at(v, 1, Tok), m, &value));
    fprintf(fdopen(fd, "w"), "%f\n", value);
    *s = Signal(None, 0);
    return Ok;
}

void
op_initOpTable()
{
    addEntry(nop);

    addEntry(mov);
    addEntry(cpy);
    addEntry(var);
    addEntry(loc);
    addEntry(incr);
    addEntry(allc);

    addEntry(add);
    addEntry(sub);
    addEntry(mul);
    addEntryAlternate(div);
    addEntry(mod);

    addEntry(eq);
    addEntry(ne);
    addEntry(gt);
    addEntry(lt);

    addEntry(and);
    addEntry(or);
    addEntry(not);

    addEntry(jmp);
    addEntry(jc);
    addEntry(lbl);

    addEntryAlternate(exit);
    addEntryAlternate(open);
    addEntryAlternate(close);
    addEntryAlternate(read);
    addEntryAlternate(write);

    addEntry(src);

    addEntry(print_num);
}

#undef addEntry

// Do NOT free Signal
// Signal.Src is owned by Code
Error
op_exec(Mem* m, const Code* c, Signal* signal)
{
    const Line* l = Code_curr(c);
    return (*Vec_at(&funcVec, l->opcode, OpFunc))(&l->toks, m, signal);
}

Error
Code_fromFile(const char* fileName, Mem* m, Code* c);

Error
Signal_respond(const Signal* self, Mem* m, Code* c)
{
    switch (self->type) {
        case Signal_None:
            break;
        case Signal_Jmp:
            Code_ptr_set(c, self->Jmp);
            Mem_mem_set(m, -1, Code_ptr(c));
            return Ok;
        case Signal_SetLbl:
            Mem_label_set(m, self->SetLbl, Code_ptr(c)+1);
            break;
        case Signal_Curr:
            Mem_mem_set(m, 0, Code_ptr(c));
            break;
        case Signal_Src:
            {
            double oldPtr;
            Mem_mem_at(m, -1, &oldPtr);
            Code src = Code_new();
            try(Code_fromFile(self->Src.array, m, &src));
            try(run(m, &src));
            Mem_mem_set(m, -1, oldPtr);
            }
            break;
    }
    Code_ptr_incr(c);
    Mem_mem_set(m, -1, Code_ptr(c));
    return Ok;
}

// Extend Tok

Error
Tok_getValue(const Tok* self, const Mem* m, double* d)
{
    switch (self->tokType) {
        case Num:
            *d = self->Num;
            return Ok;

        case Idx: {
            const struct Idx* idx = &self->Idx;
            size_t layer = 0;
            while(idx->type == Idx_Type_Idx){
                idx = idx->Idx;
                layer++;
            }

            if (idx->type == Idx_Type_Num){
                try(Mem_mem_at(m, idx->Num, d));
            }else{
                long i;
                try(Mem_var_find(m, &idx->Var, &i));
                try(Mem_mem_at(m, i, d));
                layer++;
            }

            for (size_t i = 0; i < layer; i++){
                if (*d != (long)*d)
                    return Error_NotInteger;
                try(Mem_mem_at(m, (long)*d, d));
            }
            return Ok;
            }

        case Var: {
            // Find where Var points to 
            long i;
            try(Mem_var_find(m, &self->Var, &i));
            return Mem_mem_at(m, i, d);
            }

        case Sym: {
            size_t loc;
            try(Mem_label_find(m, &self->Sym, &loc));
            *d = loc;
            return Ok;
            }

        default:
            return Error_WrongArgType;
    }
}

Error
Tok_getUint(const Tok* self, const Mem* m, size_t* i)
{
    double f;
    try(Tok_getValue(self, m, &f));
    *i = rint(f);
    if (*i != f){
        return Error_NotPositiveInteger;
    }
    return Ok;
}

Error
Tok_getInt(const Tok* self, const Mem* m, long *i)
{
    double f;
    try(Tok_getValue(self, m, &f));
    *i = rint(f);
    if (*i != f){
        return Error_NotInteger;
    }
    return Ok;
}

Error
Tok_getLoc(const Tok* self, Mem* m, long* l)
{
    switch (self->tokType){
        case Idx:
            ;const struct Idx* idx = &self->Idx;
            long layer = 0;
            while(idx->type == Idx_Type_Idx){
                idx = idx->Idx;
                layer++;
            }

            if (idx->type == Idx_Type_Num){
                *l = idx->Num;
            }else{
                long i; double d;
                try(Mem_var_find(m, &idx->Var, &i));
                try(Mem_mem_at(m, i, &d));
                if (d != (long)d) return Error_NotInteger;
                *l = d;
            }

            for (long i = 0; i < layer-1; i++){
                double d = *l;
                try(Mem_mem_at(m, (long)d, &d));
                if (d != (long)d) return Error_NotInteger;
                *l = d;
            }
            return Ok;

        case Var:
            return Mem_var_find(m, &self->Var, l);

        case Ltl:
            return Tok_createLtl(self, m, l);

        default:
            return Error_WrongArgType;

    }
}

Error
Tok_writeValue(const Tok* self, Mem* m, double d)
{
    long idx;
    try(Tok_getLoc(self, m, &idx));
    if (idx < 0){
        return Error_CannotWriteToNMem;
    }
    try(Mem_mem_set(m, idx, d));
    return Ok;
}

Error
Tok_createLtl(const Tok* self, Mem* m, long* i)
{
    if (self->tokType != Ltl){
        return Error_WrongArgType;
    }
    *i = Mem_nmem_len(m);
    for (int i = 0; i < Str_count(&self->Ltl); i++){
        Vec_push(&m->nmem, (double)*Str_at(&self->Ltl, i));
    }
    // ending a literal with two 0.0 slots, one provided by trailing null char in self.Ltl
    Vec_push(&m->nmem, 0.0);
    return Ok;
}

// Do NOT free the HashIdx
Error
Tok_getSym(const Tok* self, HashIdx* hi)
{
    if (self->tokType != Sym){
        return Error_WrongArgType;
    }
    *hi = self->Sym;
    return Ok;
}

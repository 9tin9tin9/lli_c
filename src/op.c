#include "include/op.h"
#include "include/opdef.h"
#include <stdio.h>

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

    // 1
    addEntry(mov);
    addEntry(cpy);
    addEntry(var);
    addEntry(incr);
    addEntry(decr);
    addEntry(allc);

    // 7
    addEntry(add);
    addEntry(sub);
    addEntry(mul);
    addEntryAlternate(div);
    addEntry(mod);

    // 12
    addEntry(eq);
    addEntry(ne);
    addEntry(gt);
    addEntry(lt);

    // 16
    addEntry(and);
    addEntry(or);
    addEntry(not);

    // 19
    addEntry(jmp);
    addEntry(jc);
    addEntry(lbl);
    addEntry(als);

    // 23
    addEntryAlternate(exit);
    addEntryAlternate(open);
    addEntryAlternate(close);
    addEntryAlternate(read);
    addEntryAlternate(write);

    addEntry(print_num);
}

#undef addEntry

// Do NOT free Signal
// Signal.Src is owned by Code
Error
op_exec(Mem* m, const Code* c, Signal* signal)
{
    Line* l = Code_curr(c);
    // TokType should have been checked during preprocessing already
    return (*Vec_at(&funcVec, l->opcode, OpFunc))(&l->toks, m, signal);
}

Error
readFromFile(const char* fileName, Mem* m, Code* c);

Error
Signal_respond(const Signal* self, Mem* m, Code* c)
{
    switch (self->type) {
        case None:
            break;
        case Jmp:
            Code_ptr_set(c, self->Jmp);
            return Ok;
        case SetLbl:
            Mem_label_set(m, self->SetLbl, Code_ptr(c)+1);
            break;
        case SetAls:
            Mem_label_set(m, self->SetAls.alias, self->SetAls.loc);
            break;
        case Src:
            try(readFromFile(self->Src.array, m, c));
            break;
    }
    Code_ptr_incr(c);
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

        case Idx:
            return Mem_mem_at(m, self->Idx, d);

        case Var:
            // Find where Var points to
            ;long i;
            try(Mem_var_find(m, &self->Var, &i));
            return Mem_mem_at(m, i, d);

        case VarIdx:
            // Read the value pointed by Var, use it as pointer to lookup value
            ;double value;
            try(Mem_var_find(m, &self->VarIdx, &i));
            try(Mem_mem_at(m, i, &value));
            if (value != (long)value){
                return Error_NotInteger;
            }
            return Mem_mem_at(m, value, d);

        default:
            return Error_WrongArgType;
    }
}

Error
Tok_getUint(const Tok* self, const Mem* m, size_t* i)
{
    double f;
    try(Tok_getValue(self, m, &f));
    if (f != (unsigned long)f){
        return Error_NotPositiveInteger;
    }
    *i = (size_t)f;
    return Ok;
}

Error
Tok_getInt(const Tok* self, const Mem* m, long *i)
{
    double f;
    try(Tok_getValue(self, m, &f));
    if (f != (long)f){
        return Error_NotInteger;
    }
    *i = (long)f;
    return Ok;
}

Error
Tok_getLoc(const Tok* self, Mem* m, long* l)
{
    switch (self->tokType){
        case Idx:
            *l = self->Idx;
            return Ok;

        case Var:
            return Mem_var_find(m, &self->Var, l);

        case VarIdx:
            ;long idx;
            try(Mem_var_find(m, &self->VarIdx, &idx));
            double value;
            try(Mem_mem_at(m, idx, &value));
            if (value != (long)value){
                return Error_NotInteger;
            }
            *l = (long)value;
            return Ok;

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

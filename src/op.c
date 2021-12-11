#include "include/op.h"
#include "include/opdef.h"

#define addEntry(op) { \
    Vec_push(&funcVec, &op); \
    Hashmap_insert(&opIdxTable, #op, Vec_count(funcVec)-1); \
}

Hashmap opIdxTable = Hashmap();
Vec funcVec = Vec(OpFunc);

void
op_initOpTable()
{
    addEntry(nop);
    addEntry(mov);
}

#undef addEntry

// Do NOT free Signal
// Signal.Src is owned by Code
Error
op_exec(Mem* m, Code c, Signal* result)
{
    Line* l = Code_curr(c);
    // TokType should have been checked during preprocessing already
    size_t opcode = l->opcode;
    return (*Vec_at(funcVec, opcode, OpFunc))(l->toks, m, result);
}

Error
Signal_respond(Signal self, Mem* m, Code* c)
{
    switch (self.type) {
        case None:
            break;
    }

    Code_ptr_incr(c, 1);
    return Ok;
}

// Extend Tok

Error
Tok_getValue(Tok self, Mem m, double* d)
{
    switch (self.tokType) {
        case Num:
            *d = self.Num;
            return Ok;

        case Idx:
            return Mem_mem_at(m, self.Idx, d);

        case Var:
            // Find where Var points to
            ;long i;
            Error r = Mem_var_find(m, self.Var, &i);
            if (r) return r;
            return Mem_mem_at(m, i, d);

        case VarIdx:
            // Read the value pointed by Var, use it as pointer to lookup value
            ;double value;
            r = Mem_var_find(m, self.VarIdx, &i);
            if (r) return r;
            r = Mem_mem_at(m, i, &value);
            if (r) return r;
            if (value != (long)value){
                return Error_NotInteger;
            }
            return Mem_mem_at(m, value, d);

        default:
            return Error_WrongArgType;
    }
}

Error
Tok_getUint(Tok self, Mem m, size_t* i)
{
    double f;
    Error r = Tok_getValue(self, m, &f);
    if (r) return r;
    if (f != (unsigned long)f){
        return Error_NotPositiveInteger;
    }
    *i = (long)f;
    return Ok;
}

Error
Tok_getLoc(Tok self, Mem* m, long* l)
{
    switch (self.tokType){
        case Idx:
            *l = self.Idx;
            return Ok;

        case Var:
            return Mem_var_find(*m, self.Var, l);

        case VarIdx:
            ;long idx;
            Error r = Mem_var_find(*m, self.VarIdx, &idx);
            if (r) return r;
            double value;
            r = Mem_mem_at(*m, idx, &value);
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
Tok_writeValue(Tok self, Mem* m, double d)
{
    long idx;
    Error r = Tok_getLoc(self, m, &idx);
    if (r) return r;
    if (idx < 0){
        return Error_CannotWriteToNMem;
    }
    r = Mem_mem_set(m, idx, d);
    return r;
}

Error
Tok_createLtl(Tok self, Mem* m, long* i)
{
    if (self.tokType != Ltl){
        return Error_WrongArgType;
    }
    *i = Mem_nmem_len(*m);
    for (int i = 0; i < Str_count(self.Ltl); i++){
        Vec_push(&m->nmem, (double)*Str_at(self.Ltl, i));
    }
    // ending a literal with two 0.0 slots, one provided by trailing null char in self.Ltl
    Vec_push(&m->nmem, 0.0);
    return Ok;
}

// Do NOT free the HashIdx
Error
Tok_getSym(Tok self, HashIdx* hi)
{
    if (self.tokType != Sym){
        return Error_WrongArgType;
    }
    *hi = self.Sym;
    return Ok;
}

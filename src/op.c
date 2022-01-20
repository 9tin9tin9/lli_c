#include "include/op.h"
#include "include/opdef.h"
#include "include/core.h"
#include <stdio.h>
#include <math.h>

Hashmap opIdxTable = Hashmap();
Vec funcVec = Vec(OpFunc);

Error
print_num(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    size_t fd;
    Value value;
    try(Tok_getUint(Vec_at(v, 0, Tok), m, &fd));
    try(Tok_getValue(Vec_at(v, 1, Tok), m, &value));
    if (value.type == 'D'){
        fprintf(fdopen(fd, "w"), "%f\n", value.Double);
    }else{
        fprintf(fdopen(fd, "w"), "%ld\n", value.Long);
    }
    *s = Signal(None, 0);
    return Ok;
}

// Do NOT free Signal
// Signal.Src is owned by Code
Error
op_exec(Mem* m, const Code* c, Signal* signal)
{
    const Line* l = Code_curr(c);
    return (*Vec_at_unsafe(&funcVec, l->opcode, OpFunc))(&l->toks, m, signal);
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
            return Ok;
        case Signal_SetLbl:
            Mem_label_set(m, self->SetLbl, Code_ptr(c)+1);
            break;
        case Signal_Curr:
            Mem_mem_set(m, 0, &Value(Long, Code_ptr(c)));
            break;
    }
    Code_ptr_incr(c);
    return Ok;
}

// Extend Tok

Error
Tok_getValue(const Tok* self, const Mem* m, Value* d)
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
                if (d->type != 'L')
                    return Error_NotInteger;
                try(Mem_mem_at(m, d->Long, d));
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
            *d = Value(Long, loc);
            return Ok;
            }

        default:
            return Error_WrongArgType;
    }
}

Error
Tok_getUint(const Tok* self, const Mem* m, size_t* i)
{
    long l;
    try(Tok_getInt(self, m, &l));
    if (l < 0){
        return Error_NotPositiveInteger;
    }
    *i = l;
    return Ok;
}
Error
Tok_getInt(const Tok* self, const Mem* m, long *i)
{
    Value f;
    try(Tok_getValue(self, m, &f));
    if (f.type != 'L'){
        return Error_NotInteger;
    }
    *i = f.Long;
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
                long i; Value d;
                try(Mem_var_find(m, &idx->Var, &i));
                try(Mem_mem_at(m, i, &d));
                *l = d.Long;
            }

            for (long i = 0; i < layer-1; i++){
                Value d = Value(Long, *l);
                try(Mem_mem_at(m, d.Long, &d));
                *l = d.Long;
            }
            return Ok;

        case Var:
            return Mem_var_find(m, &self->Var, l);

        case Ltl:
            *l = -self->Ltl.idx;
            return Ok;

        default:
            return Error_WrongArgType;

    }
}

Error
Tok_writeValue(const Tok* self, Mem* m, Value* d)
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
Tok_createLtl(const Tok* self, Mem* m, size_t* i)
{
    *i = Mem_mem_len(m);
    for (int i = 0; i < Str_count(&self->Ltl.sym); i++){
        Vec_push(&m->mem, Value(Long, *Str_at(&self->Ltl.sym, i)));
    }
    Vec_push(&m->mem, Value(Long, 0));
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

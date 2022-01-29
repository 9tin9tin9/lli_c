#include "include/op.h"
#include "include/opdef.h"
#include "include/core.h"
#include <stdio.h>
#include <math.h>

Hashmap opIdxTable = Hashmap();

// Extend Tok

Error
Tok_getValueNum(const Tok* self, const Mem* m, Value* d)
{
    *d = self->Num;
    return Ok;
}

Error
Tok_getValueVar(const Tok* self, const Mem* m, Value* d)
{
    long i;
    try(Mem_var_find(m, &self->Var, &i));
    return Mem_mem_at(m, i, d);
}

Error
Tok_getValueIdx(const Tok* self, const Mem* m, Value* d)
{
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

Error
Tok_getValueSym(const Tok* self, const Mem* m, Value* d)
{
    size_t loc;
    try(Mem_label_find(m, &self->Sym, &loc));
    *d = Value(Long, loc);
    return Ok;
}

Error
Tok_getLocVar(const Tok* self, Mem* m, long* l)
{
    return Mem_var_find(m, &self->Var, l);
}

Error
Tok_getLocIdx(const Tok* self, Mem* m, long* l)
{
    const struct Idx* idx = &self->Idx;
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
}

Error
Tok_getLocLtl(const Tok* self, Mem* m, long* l)
{
    *l = -self->Ltl.idx;
    return Ok;
}

Error
Tok_writeValueVar(const Tok* self, Mem* m, Value* d)
{
    long idx;
    try(Tok_getLocVar(self, m, &idx));
    if (idx < 0){
        return Error_CannotWriteToNMem;
    }
    try(Mem_mem_set(m, idx, d));
    return Ok;
}

Error
Tok_writeValueIdx(const Tok* self, Mem* m, Value* d)
{
    long idx;
    try(Tok_getLocIdx(self, m, &idx));
    if (idx < 0){
        return Error_CannotWriteToNMem;
    }
    try(Mem_mem_set(m, idx, d));
    return Ok;
}

Error
Tok_writeValueLtl(const Tok* self, Mem* m, Value* d)
{
    long idx;
    try(Tok_getLocVar(self, m, &idx));
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

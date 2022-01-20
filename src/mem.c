#include "include/mem.h"
#include <limits.h>
#include <stdio.h>

Mem
Mem_new(size_t reg)
{
    bool fd[1024] = { false };
    fd[0] = fd[1] = fd[2] = true;
    Mem mem = (Mem){
        .mem = Vec(Value),
        .varLookUp = Hashmap(),
        .var = Vec(long),
        .labelLookUp = Hashmap(),
        .label = Vec(size_t),
        .fd = _Vec_from(1024, sizeof(bool), fd),
    };
    Mem_mem_push(&mem, reg+2);
    Hashmap_insert(&mem.varLookUp, "?", Mem_var_add(&mem, 0));
    Hashmap_insert(&mem.varLookUp, "*", Mem_var_add(&mem, 1));
    // declaring numbered registers
    for (int i = 0; i < reg; i++){
        char ch[1024];
        sprintf(ch, "%d", i+1);
        // [1] reserved for current line number
        Hashmap_insert(&mem.varLookUp, ch, Mem_var_add(&mem, i+2));
    }
    return mem;
}

Error
Mem_mem_at(const Mem* self, long i, Value* des)
{
    if (i < 0) i = -i;
    Value* a = Vec_at(&self->mem, i, Value);
    if (!a)
        return Error_InvalidMemAccess;
    *des = *a;
    return Ok;
}

Error
Mem_mem_set(Mem* self, long i, Value* v)
{
    if (i < 0) i = -i;
    Value* a = Vec_at(&self->mem, i, Value);
    if (!a)
        return Error_InvalidMemAccess;
    *a = *v;
    return Ok;
}

size_t
Mem_mem_len(const Mem* self)
{
    return self->mem.size;
}

void
Mem_mem_push(Mem* self, size_t size)
{
    self->mem.array = realloc(
            self->mem.array,
            (self->mem.size+size)*self->mem.elem_size);
    for (size_t i = self->mem.size; i < self->mem.size+size; i++){
        memcpy(
            Vec_at_unsafe(&self->mem, i, Value),
            &Value(Long, 0),
            self->mem.elem_size);
    }
    self->mem.size += size;
}

size_t
Mem_var_add(Mem* self, long idx)
{
    Vec_push(&self->var, idx);
    return Vec_count(&self->var) - 1;
}

void
Mem_var_set(Mem* self, size_t i, long idx)
{
    *Vec_at(&self->var, i, long) = idx;
}

Error
Mem_var_find(const Mem* self, const HashIdx* hi, long* des)
{
    // hi->idx should have been assigned during preprocessing
    *des = *Vec_at_unsafe(&self->var, hi->idx, long);

    // allowing resolving symbols during runtime
    // long* v = Vec_at(&self->var, hi->idx, long);
    // if (!v)
    //     return Error_UndefinedVar;
    // *des = *v;
    return Ok;
}

size_t
Mem_label_add(Mem* self, size_t idx)
{
    Vec_push(&self->label, idx);
    return Vec_count(&self->label) - 1;
}

void
Mem_label_set(Mem* self, size_t i, size_t idx)
{
    *Vec_at(&self->label, i, size_t) = idx;
}

Error
Mem_label_find(const Mem* self, const HashIdx* hi, size_t* des)
{
    // hi->idx should have been assigned during preprocessing
    *des = *Vec_at_unsafe(&self->label, hi->idx, size_t);

    // allowing resolving symbols during runtime
    // size_t* l = Vec_at(&self->label, hi->idx, size_t);
    // if (!l)
    //     return Error_UndefinedLabel;
    // *des = *l;
    return Ok;
}

Error
Mem_readLtl(const Mem* self, long idx, Str* des)
{
    while(1){
        Value c;
        try(Mem_mem_at(self, idx, &c));
        if (c.Long == 0){
            return Ok;
        }
        Str_push(des, *Value_getL(&c));
        idxIncr(&idx, 1);
    }
}

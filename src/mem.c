#include "include/mem.h"
#include <limits.h>

Mem
Mem_new()
{
    bool fd[1024] = { false };
    fd[0] = fd[1] = fd[2] = true;
    return (Mem){
        .pmem = Vec_from(Value, Value(Long, 0)),
        .nmem = Vec(Value),
        .varLookUp = Hashmap(),
        .var = Vec(long),
        .labelLookUp = Hashmap(),
        .label = Vec(size_t),
        .fd = _Vec_from(1024, sizeof(bool), fd),
    };
}

Error
Mem_mem_at(const Mem* self, long i, Value* des)
{
    return i < 0 ? 
        Mem_nmem_at(self, -i, des) : 
        Mem_pmem_at(self, i, des);
}

Error
Mem_mem_set(Mem* self, long i, Value v)
{
    return i < 0 ?
        Mem_nmem_set(self, -i, v) :
        Mem_pmem_set(self, i, v);
}

Error
Mem_pmem_at(const Mem* self, size_t i, Value* des)
{
    Value* slot = Vec_at(&self->pmem, i, Value);
    if (!slot) 
        return Error_InvalidMemAccess;
    *des = *slot;
    return Ok;
}

Error
Mem_pmem_set(Mem* self, size_t i, Value v)
{
    Value* slot = Vec_at(&self->pmem, i, Value);
    if (!slot)
        return Error_InvalidMemAccess;
    *slot = v;
    return Ok;
}

size_t
Mem_pmem_len(const Mem* self)
{
    return Vec_count(&self->pmem);
}

void
Mem_pmem_push(Mem* self, Value v)
{
    Vec_push(&self->pmem, v);
}

Error
Mem_nmem_at(const Mem* self, size_t i, Value* des)
{
    Value* slot = Vec_at(&self->nmem, i-1, Value);
    if (!slot)
        return Error_InvalidMemAccess;
    *des = *slot;
    return Ok;
}

Error
Mem_nmem_set(Mem* self, size_t i, Value v)
{
    Value* slot = Vec_at(&self->nmem, i-1, Value);
    if (!slot)
        return Error_InvalidMemAccess;
    *slot = v;
    return Ok;
}

size_t
Mem_nmem_len(const Mem* self)
{
    return Vec_count(&self->nmem);
}

void
Mem_nmem_alloc(Mem* self, const Vec* v){
    Vec_insert(&self->nmem, v, Vec_count(&self->nmem));
}

void
Mem_nmem_push(Mem* self, Value v)
{
    Vec_push(&self->nmem, v);
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
    size_t* elem = Vec_at(&self->label, hi->idx, size_t);
    if (!elem)
        return Error_UndefinedLabel;
    *des = *elem;
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

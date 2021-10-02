#include "include/mem.h"

Mem
Mem_new()
{
    return (Mem){
        .pmem = Vec(double),
        .nmem = Vec_from(double, 0.0),
        .varLookUp = Hashmap(),
        .var = Vec(long),
        .labelLookUp = Hashmap(),
        .label = Vec(long),
        .fd = Vec(bool),
    };
}

Error
Mem_mem_at(Mem self, long i, double* des)
{
    return i < 0 ? 
        Mem_nmem_at(self, -i, des) : 
        Mem_pmem_at(self, i, des);
}

Error
Mem_mem_set(Mem* self, long i, double v)
{
    return i < 0 ?
        Mem_nmem_set(self, -i, v) :
        Mem_pmem_set(self, i, v);
}

Error
Mem_pmem_at(Mem self, size_t i, double* des)
{
    double* slot = Vec_at(self.pmem, i, double);
    if (!slot) 
        return Error_InvalidMemAccess;
    *des = *slot;
    return Ok;
}

Error
Mem_pmem_set(Mem* self, size_t i, double v)
{
    double* slot = Vec_at(self->pmem, i, double);
    if (!slot)
        return Error_InvalidMemAccess;
    *slot = v;
    return Ok;
}

size_t
Mem_pmem_len(Mem self)
{
    return Vec_count(self.pmem);
}

void
Mem_pmem_push(Mem* self, double v)
{
    Vec_push(&self->pmem, v);
}

Error
Mem_nmem_at(Mem self, size_t i, double* des)
{
    double* slot = Vec_at(self.nmem, i, double);
    if (!slot)
        return Error_InvalidMemAccess;
    *des = *slot;
    return Ok;
}

Error
Mem_nmem_set(Mem* self, size_t i, double v)
{
    double* slot = Vec_at(self->nmem, i, double);
    if (!slot)
        return Error_InvalidMemAccess;
    *slot = v;
    return Ok;
}

size_t
Mem_nmem_len(Mem self)
{
    return Vec_count(self.nmem);
}

void
Mem_nmem_alloc(Mem* self, Vec v){
    Vec_insert(&self->nmem, v, Vec_count(self->nmem));
}

void
Mem_nmem_push(Mem* self, double v)
{
    Vec_push(&self->nmem, v);
}

size_t
Mem_var_add(Mem* self, long idx)
{
    Vec_push(&self->var, idx);
    return Vec_count(self->var) - 1;
}

void
Mem_var_set(Mem* self, size_t i, long idx)
{
    *Vec_at(self->var, i, long) = idx;
}

Error
Mem_var_find(Mem self, HashIdx hi, long* des)
{
    long* elem = Vec_at(self.var, hi.idx, long);
    if (!elem)
        return Error_UndefinedVar;
    *des = *elem;
    return Ok;
}

size_t
Mem_label_add(Mem* self, long idx)
{
    Vec_push(&self->label, idx);
    return Vec_count(self->label) - 1;
}

void
Mem_label_set(Mem* self, size_t i, long idx)
{
    *Vec_at(self->label, i, long) = idx;
}

Error
Mem_label_find(Mem self, HashIdx hi, long* des)
{
    long* elem = Vec_at(self.label, hi.idx, long);
    if (!elem)
        return Error_UndefinedLabel;
    *des = *elem;
    return Ok;
}

void
idxIncr(long* i, long delta)
{
    *i += (*i >= 0? delta : -delta);
}

void
idxDecr(long* i, long delta)
{
    *i -= (*i >= 0? delta : -delta);
}

Error
Mem_readLtl(Mem self, long idx, Str* des)
{
    int zeroCount = 0;
    while(1){
        double c;
        Error r = Mem_mem_at(self, idx, &c);
        if (r) return r;
        if (c == 0.0){
            if (++zeroCount == 2){
                return Ok;
            }
        }else{
            zeroCount = 0;
        }
        Str_push(des, (char)c);
        idxIncr(&idx, 1);
    }
}

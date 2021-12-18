#ifndef _MEM_H_
#define _MEM_H_

#include "container/vec.h"
#include "container/hashmap.h"
#include "lex.h"

typedef struct{
    Vec pmem;  // Vec<double>
    Vec nmem;  // Vec<double>  Starts at idx:1
    Hashmap varLookUp;  // Hashmap<char*, size_t>  Store index of variable
    Vec var;  // Vec<long>  Store indices that var points to
    Hashmap labelLookUp;  // Hashmap<char*, size_t>
    Vec label;  // Vec<size_t>  Store line number without empty lines
    Vec fd;  // Vec<bool>
}Mem;

Mem Mem_new();

Error Mem_mem_at(Mem, long, double*);
Error Mem_mem_set(Mem*, long, double);

Error Mem_pmem_at(Mem, size_t, double*);
Error Mem_pmem_set(Mem*, size_t, double);
size_t Mem_pmem_len(Mem);
void Mem_pmem_push(Mem*, double);

Error Mem_nmem_at(Mem, size_t, double*);
Error Mem_nmem_set(Mem*, size_t, double);
size_t Mem_nmem_len(Mem);
// Vec will be COPIED
void Mem_nmem_alloc(Mem*, Vec);
void Mem_nmem_push(Mem*, double);

// returns the index of inserted var
size_t Mem_var_add(Mem*, long);
// assume index is always valid
void Mem_var_set(Mem*, size_t, long);
Error Mem_var_find(Mem, HashIdx, long*);

// returns the index of inserted label
size_t Mem_label_add(Mem*, size_t);
// assume index is always valid
void Mem_label_set(Mem*, size_t, size_t);
Error Mem_label_find(Mem, HashIdx, size_t*);

void idxIncr(long*, long);
void idxDecr(long*, long);
Error Mem_readLtl(Mem, long, Str*);


#endif

#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#ifndef __cplusplus

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* 
 * key designed to be used on strings only
 */

/*
 * Public Types:
 *      Hashmap
 *      Hashmap_Pair
 *
 * Public Functions:
 *      Hashmap()
 *      Hashmap_Pair()
 *      Hashmap_change_hashfunc(h, func)
 *      Hashmap_count(h)
 *      Hashmap_rehash(h, grow_size)
 *      Hashmap_at(h, key)
 *      Hashmap_insert(h, key, val)
 *      Hashmap_remove(h, key)
 *      Hashmap_contains(h, key)
 */

// default hash function
static inline unsigned long 
sdbm(const char* str, size_t size)
{
    unsigned long hash = 0;
    size_t i = 0;
    for (size_t i = 0; i < size; i++){
        int c = *str++;
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

typedef unsigned long (*Hashfunc)(const char*, size_t);

#ifndef HASHMAP_DEFAULT_SIZE
#define HASHMAP_DEFAULT_SIZE 95791
#endif

#ifndef HASHMAP_GROWTH_FACTOR
#define HASHMAP_GROWTH_FACTOR 100
#endif

typedef struct{
    char* key;
    size_t key_s;
    size_t val;
}Hashmap_Pair;

static inline Hashmap_Pair
_Hashmap_Pair(const void* key, size_t key_s, size_t val)
{
    Hashmap_Pair p = (Hashmap_Pair){
        malloc(key_s),
        key_s,
        val,
    };
    memcpy(p.key, key, key_s);
    return p;
}

#define Hashmap_Pair(k, v) \
    _Hashmap_Pair(k, strlen(k)+1, v)

static inline void
_Hashmap_Pair_del(Hashmap_Pair* p)
{
    free(p->key);
    memset(p, 0, sizeof(Hashmap_Pair));
}

typedef struct Hashmap{
    size_t cap;
    Hashmap_Pair* array;
    Hashfunc hashfunc;
}Hashmap;

#define Hashmap() ((Hashmap){ \
    HASHMAP_DEFAULT_SIZE, \
    NULL, \
    sdbm \
})

static inline void
Hashmap_change_hashfunc(Hashmap* h, Hashfunc f){
    h->hashfunc = f;
}

static inline size_t
Hashmap_count(Hashmap h)
{
    size_t size = 0;
    for (size_t i = 0; i < h.cap; i++){
        size += h.array[i].key_s != 0;
    }
    return size;
}

static inline void
_Hashmap_insert(Hashmap*, Hashmap_Pair);

static inline void
Hashmap_rehash(Hashmap* h, size_t growth_factor)
{
    Hashmap* new = malloc(sizeof(Hashmap));
    *new = (Hashmap){
        h->cap + growth_factor,
        calloc(h->cap + growth_factor, sizeof(Hashmap_Pair)),
        h->hashfunc,
    };
    for (size_t i = 0; i < h->cap; i++){
        _Hashmap_insert(new, h->array[i]);
    }
    free(h->array);
    *h = *new;
}

static inline size_t
_Hashmap_get_idx(Hashmap h, const char* key, size_t key_s)
{
    return h.hashfunc(key, key_s) % h.cap;
}

static inline void
_Hashmap_insert(Hashmap* h, Hashmap_Pair pair)
{
    if (h->array == NULL){
        h->array = calloc(h->cap, sizeof(Hashmap_Pair));
    }
    size_t idx = _Hashmap_get_idx(*h, pair.key, pair.key_s);
    if (h->array[idx].key_s != 0){
        Hashmap_rehash(h, HASHMAP_GROWTH_FACTOR);
    }
    h->array[idx] = pair;
}

#define Hashmap_insert(h, k, v) \
    _Hashmap_insert(h, Hashmap_Pair(k, v))

static inline void
_Hashmap_remove(Hashmap* h, const void* key, size_t key_s)
{
    if (h->array == NULL) return;
    size_t idx = _Hashmap_get_idx(*h, key, key_s);
    _Hashmap_Pair_del(&h->array[idx]);
}

#define Hashmap_remove(h, k)\
    _Hashmap_remove(h, (k), strlen(k)+1)

static inline bool
_Hashmap_contains(Hashmap h, const void* key, size_t key_s)
{
    if (h.array == NULL) return false;
    size_t idx = _Hashmap_get_idx(h, key, key_s);
    return 
        h.array[idx].key_s != 0 && 
        memcmp(h.array[idx].key, key, key_s) == 0;
}

#define Hashmap_contains(h, k)\
    _Hashmap_contains(h, k, strlen(k)+1)

static inline void*
_Hashmap_at(Hashmap h, const void* key, size_t key_s)
{
    if (h.array == NULL) return NULL;
    Hashmap_Pair* pair = &h.array[_Hashmap_get_idx(h, key, key_s)];
    if (pair->key_s == key_s && memcmp(pair->key, pair->key, key_s) == 0){
        return &pair->val;
    }
    return NULL;
}

#define Hashmap_at(h, k, t)\
    (t*)_Hashmap_at(h, k, strlen(k)+1)

#endif

#endif

#ifndef _STR_H_
#define _STR_H_

#include <stdlib.h>
#include <string.h>

// NULL terminated
typedef struct {
    size_t size;
    char* array;
}Str;

#define Str() ((Str){ 1, memset(malloc(1), 0, 1) })

#define Str_fromLtl(s) ((Str){ \
    sizeof(__typeof__(s)), \
    strdup(s) })

static inline Str
Str_fromPtr(const char* s, size_t len)
{
    return (Str){
        len,
        strdup(s),
    };
}

static inline Str
Str_copy(const Str* s)
{
    return Str_fromPtr(s->array, s->size);
}

// strlen
#define Str_len(s) ((s)->size-1)
// array size
#define Str_count(s) ((s)->size)

// return NULL if out of range
static inline char*
Str_at(const Str* s, size_t idx)
{
    if (idx >= s->size) return NULL;
    return s->array + idx;
}

#define Str_raw(s) ((s)->array)

#define Str_front(s) (*(s)->array)

// character before nul
static inline short
Str_back(const Str* s)
{
    if (s->size <= 1) return -1;
    return s->array[s->size-2];
}

static inline void
Str_push(Str* s, char c)
{
    s->size++;
    s->array = realloc(s->array, s->size);
    s->array[s->size-2] = c;
    s->array[s->size-1] = 0;
}

static inline void
Str_pop(Str* s)
{
    s->size--;
    s->array = realloc(s->array, s->size);
    s->array[s->size-1] = 0;
}

static inline void
Str_del(Str* s)
{
    s->size = 0;
    free(s->array);
}

#endif

#ifndef _TUPLE_H_
#define _TUPLE_H_

#ifndef __cplusplus

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

/* 
 * Public Types:
 *      Tuple
 *
 * Public Functions:
 *      Tuple(val...)
 *      Tuple_at(tuple, pos, type)
 *      Tuple_count(tuple)
 */

typedef struct _Tuple{
    size_t count;
    char* array;
    size_t sizes[10];
}Tuple;

// count __VA_ARGS__ from: https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments

#define _COUNT(...) \
            _COUNT_(__VA_ARGS__, _NUM())
#define _COUNT_(...) \
            _DISPLACE(__VA_ARGS__)
#define _DISPLACE( \
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define _NUM() \
            10, 9, 8, 7, 6, 5, 4, 3, 2, 1

// for each from: https://stackoverflow.com/questions/1872220/is-it-possible-to-iterate-over-arguments-in-variadic-macros

#define _M9(map_, x, ...) map_(x) _M8(map_, __VA_ARGS__)
#define _M8(map_, x, ...) map_(x) _M7(map_, __VA_ARGS__)
#define _M7(map_, x, ...) map_(x) _M6(map_, __VA_ARGS__)
#define _M6(map_, x, ...) map_(x) _M5(map_, __VA_ARGS__)
#define _M5(map_, x, ...) map_(x) _M4(map_, __VA_ARGS__)
#define _M4(map_, x, ...) map_(x) _M3(map_, __VA_ARGS__)
#define _M3(map_, x, ...) map_(x) _M2(map_, __VA_ARGS__)
#define _M2(map_, x, ...) map_(x) _M1(map_, __VA_ARGS__)
#define _M1(map_, x) map_(x)

#define _CONCAT(a, b) a##b
#define _MAP_(N, map, ...) _CONCAT(_M, N)(map, __VA_ARGS__)
#define _MAP(map, ...) _MAP_(_COUNT(__VA_ARGS__), map, __VA_ARGS__)

#define _wrap_sizeof(x) sizeof(__typeof__(x)), 
#define _sum_sizeof(x) sizeof(__typeof__(x))+

// Tuple can only store up to 9 elements
#define _Tuple_create_empty(...) ( \
    (Tuple){ \
        .count = _COUNT(__VA_ARGS__), \
        .array = malloc( _MAP(_sum_sizeof, __VA_ARGS__) 0 ), \
        .sizes = { _MAP(_wrap_sizeof, __VA_ARGS__) 0 }, \
    } \
)

#define _copy_elem(x) \
    memcpy(t_.array, (__typeof__(x)[]){x}, t_.sizes[i]); \
    i++; \
    t_.array += t_.sizes[i]; 

static inline Tuple __Tuple_init(Tuple tuple, int argc, ...){
    va_list ap;
    char* array = tuple.array;
    va_start(ap, argc);
    for (int i = 0; i < argc; i++) {
        memcpy((void*)array, va_arg(ap, void*), tuple.sizes[i]);
        array += tuple.sizes[i];
    }
    va_end(ap);
    return tuple;
}

#define _wrap_temp_array(x) (__typeof__(x)[]){x}, 

#define _Tuple_init(tuple_, N, ...) ( \
    __Tuple_init((tuple_), N, _MAP(_wrap_temp_array, __VA_ARGS__) 0) \
)

#define Tuple(...) _Tuple_init(\
        _Tuple_create_empty(__VA_ARGS__), \
        _COUNT(__VA_ARGS__), \
        __VA_ARGS__)

#define Tuple_count(t_) ( (t_).count )

// returns NULL if tuple.array == NULL or pos out of range
static inline void* _Tuple_at(Tuple tuple, size_t pos){
    if (tuple.array == 0 || pos > tuple.count)
        return NULL;
    char* array = tuple.array;
    for (int i = 0; i < pos; i++){
        array += tuple.sizes[i];
    }
    return (void*)array;
}

#define Tuple_at(tuple_, pos_, type_) ((type_)_Tuple_at((tuple_), pos_))

static inline void Tuple_del(Tuple* tuple){
    free(tuple->array);
    *tuple = (Tuple){ 0, 0, {0} };
}


#endif

#endif

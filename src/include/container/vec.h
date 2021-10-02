#ifndef _VEC_H_ 
#define _VEC_H_

#ifndef __cplusplus

#include <stdlib.h>
#include <string.h>

/*
 * Public Types:
 *      Vec
 *
 * Public Functions:
 *      Vec(type)
 *      Vec_count(v)
 *      Vec_size(v)
 *      Vec_from(type, elem...)
 *      Vec_copy(src)
 *      Vec_del(v)
 *      Vec_at(v, i, t)
 *      Vec_front(v, t)
 *      Vec_back(v, t)
 *      Vec_push(v, e)
 *      Vec_pop(v)
 *      Vec_insert(des, src, pos)
 *      Vec_erase(v, e)
 *      Vec_remove(v, e)
 *      Vec_clear(v)
 */

typedef struct Vec{
    size_t elem_size;
    size_t size;
    char* array;
}Vec;

// raw array size in bytes
static inline size_t Vec_size(Vec vec) { return vec.size*vec.elem_size; }

#define Vec(type_) ( (Vec){ \
    sizeof(type_), \
    0, \
    NULL \
    } \
)

#define Vec_count(v_) ( (v_).size )

static inline Vec Vec_copy(Vec src) {
    Vec v = src;
    v.array = malloc(Vec_size(src));
    memcpy(v.array, src.array, Vec_size(src));
    return v;
}

static inline Vec _Vec_from(size_t size, size_t elem_size, const void* array) {
    Vec v = {
        elem_size,
        size,
        malloc(elem_size*size)
    };
    memcpy(v.array, array, elem_size*size);
    return v;
}

#define Vec_from(type_, ...) \
        _Vec_from(\
            sizeof((type_[]){__VA_ARGS__})/sizeof(type_), \
            sizeof(type_), \
            (type_[]){__VA_ARGS__})
        

static inline void Vec_del(Vec* v_) {
    free(v_->array); 
    v_->elem_size = v_->size = 0;
    v_->array = 0; 
}

// return NULL if pos out of range
static inline void* _Vec_at(Vec v_, long p_) {
    if (p_ == -1) p_ = v_.size-1;
    if (p_ < 0 || p_ >= (v_).size) return NULL;
    return (void*)(v_.array+p_*v_.elem_size);
}

#define Vec_at(vec_, pos_, type_) ( \
    (type_*)_Vec_at(vec_, pos_) \
)

#define Vec_front(vec_, type_) Vec_at((vec_), 0, type_) 

#define Vec_back(vec_, type_) Vec_at((vec_), -1, type_) 


// return 0 if different elem sizes
// return 1 if success
static inline int _Vec_push(Vec* v_, const void* elem, size_t elem_size) {
    // different elem sizes
    if (elem_size != v_->elem_size) return 0;
    v_->size++;
    v_->array = realloc(v_->array, Vec_size(*v_));
    memcpy(_Vec_at(*v_, -1), elem, elem_size);
    return 1;
}

#define Vec_push(vec_, elem_) _Vec_push(vec_, (__typeof__((elem_))[]){(elem_)}, sizeof(__typeof__((elem_)))) 

static inline void Vec_pop(Vec* v_) {
    v_->size--;
    v_->array = realloc(v_->array, Vec_size(*v_));
}

// return 0 if either different elem sizes or pos out of range
// return 1 if success
static inline int _Vec_insert(Vec* v_, Vec a_, size_t p_) {
    // different elem sizes
    if (v_->elem_size != a_.elem_size) return 0;
    // pos out of range
    if (p_ > v_->size) return 0;
    size_t temp_size = a_.size < (v_->size - p_) ? a_.size : (v_->size - p_);
    void* temp = malloc(temp_size*v_->elem_size);
    memcpy(temp, _Vec_at(*v_, p_), temp_size*v_->elem_size);
    v_->size += a_.size;
    v_->array = realloc(v_->array, Vec_size(*v_));
    memcpy(_Vec_at(*v_, p_+a_.size), temp, temp_size*v_->elem_size);
    free(temp);
    memcpy(_Vec_at(*v_, p_), a_.array, Vec_size(a_));
    return 1;
}

#define Vec_insert(vec_, arr_, pos_) _Vec_insert(vec_, (arr_), (pos_)) 

// return 0 if different elem sizes
// return 1 if success
static inline int _Vec_erase(Vec* v, const void* elem, size_t size){
    if (v->elem_size != size) return 0;
    for (int i = 0; i < v->size; i++){
        if (!memcmp(_Vec_at(*v, i), elem, size)){
            for (int j = i; j < v->size-1; j++){
                memcpy(_Vec_at(*v, j), _Vec_at(*v, j+1), size);
            }
            v->size--;
            v->array = realloc(v->array, Vec_size(*v));
            return 1;
        }
    }
    return 1;
}

#define Vec_erase(v_, elem_) _Vec_erase(v_, (__typeof__((elem_))[]){(elem_)}, sizeof(__typeof__((elem_))))

// return 0 if pos out of range
// return 1 if success
static inline int Vec_remove(Vec* v, size_t pos){
    if (pos >= v->size) return 0;
    for (size_t i = pos; i < v->size-1; i++){
        memcpy(_Vec_at(*v, i), _Vec_at(*v, i+1), v->elem_size);
    }
    v->size--;
    v->array = realloc(v->array, v->size);
    return 1;
}

static inline void Vec_clear(Vec* v_) {
    v_->size = 0;
    v_->array = realloc(v_->array, 0);
}

#endif

#endif

#ifndef _BOX_H_
#define _BOX_H_

#ifndef __cplusplus

#include <stdlib.h>
#include <string.h>

/*
 * Public Types:
 *      Box
 *
 * Public Functions:
 *      Box(val)
 *      Box_raw_ptr(box)
 *      Box_cast_ptr(box, type)
 *      Box_realloc(box, val)
 *      Box_copy_size(box, des, size)
 *      Box_free(box)
 */

typedef struct Box{
    size_t size;
    void* value;
}Box;

#define Box(value_) ( \
    (Box) { \
        sizeof(__typeof__(value_)), \
        memcpy(malloc(sizeof(__typeof__(value_))), (__typeof__(value_)[]){value_}, sizeof(__typeof__(value_))) \
    })

static inline void* Box_raw_ptr(Box box){
    return box.value;
}

#define Box_cast_ptr(box_, type_) ((type_)Box_raw_ptr(box_))

// return NULL if box.value == NULL or read size > box.size
// else return des
static inline void* Box_copy_size(Box box, void* des, size_t size){
    if (box.value == NULL || size > box.size) return NULL;
    return memcpy(des, box.value, size);
}

static inline void Box_free(Box* box){
    free(box->value);
    box->value = NULL;
}

// Old ptr will be freed
static inline Box _Box_realloc(Box* box, void* src, size_t size){
    Box_free(box);
    box->value = memcpy(realloc(box->value, size), src, size);
    return *box;
}

#define Box_realloc(box_, value_) _Box_realloc(box_, (__typeof__(value_)[]){value_}, sizeof(__typeof__(value_)));

#endif

#endif

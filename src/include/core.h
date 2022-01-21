#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include "error.h"
#include "mem.h"
#include "code.h"

// Initialisers
typedef Error (*Generator)(void* state, char** str);

Error Code_from(
        Mem* m,
        Code* c,
        void* state);

Error generator_File(void* state, char** str);
#define Generator_File(f) (struct _{Generator _1; void* _2; int _3;}[]) \
        {(struct _){generator_File, f, 0}}

// strArr end with NULL pointer
Error generator_StrArr(void* state, char** str);
#define Generator_StrArr(f) (struct _{Generator _1; void* _2; int _3;}[]) \
        {(struct _){generator_StrArr, f, 0}}


Error resolveSym(Mem* m, Code* c);

#endif

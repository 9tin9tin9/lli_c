#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include "error.h"
#include "mem.h"
#include "code.h"

// Initialisers
Error Code_fromFile(const char* fileName, Mem* m, Code* c);
Error Code_fromStr(const char** strs, const size_t* lens, size_t len, Mem* m, Code* c);

Error run(Mem* m, Code* c);

#endif

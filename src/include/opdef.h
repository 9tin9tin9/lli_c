#ifndef _OPDEF_H_
#define _OPDEF_H_

#include "op.h"

// argc indexes start with 1
// argv[0] is the tok of current op
#define argcGuard(v_, n_) { \
    if (v_.size-1 != (n_)) { \
        return Error_WrongArgCount;\
    } \
}

defOp(nop);

defOp(mov);
defOp(cpy);

#endif

#ifndef _OPDEF_H_
#define _OPDEF_H_

#include "op.h"

#define argcGuard(v_, n_) { \
    if (v_.size != (n_)) { \
        return Error_WrongArgCount;\
    } \
}

defOp(nop);

defOp(mov);
defOp(cpy);
defOp(allc);

#endif

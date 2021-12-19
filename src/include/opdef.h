#ifndef _OPDEF_H_
#define _OPDEF_H_

#include "op.h"

#define defOp(name) Error name(Vec, Mem*, Signal*)

#define argcGuard(v_, n_) { \
    if (v_.size != (n_)) { \
        return Error_WrongArgCount;\
    } \
}

defOp(nop);

defOp(mov);
defOp(cpy);
defOp(var);
defOp(incr);
defOp(decr);
defOp(allc);

defOp(add);
defOp(sub);
defOp(mul);
defOp(div_);
defOp(mod);

defOp(eq);
defOp(ne);
defOp(gt);
defOp(lt);

defOp(and);
defOp(or);
defOp(not);

defOp(jmp);
defOp(jc);
defOp(lbl);
defOp(als);

defOp(exit_);
defOp(open_);
defOp(close_);
defOp(read_);
defOp(write_);

#endif

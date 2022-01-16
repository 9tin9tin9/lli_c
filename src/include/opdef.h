#ifndef _OPDEF_H_
#define _OPDEF_H_

#include "op.h"

#define defOp(name) Error name(const Vec*, Mem*, Signal*)

#define argcGuard(v_, n_) { \
    if (v_->size != (n_)) { \
        return Error_WrongArgCount;\
    } \
}

enum Opcode{
    OPCODE_NOP,

    OPCODE_MOV,
    OPCODE_CPY,
    OPCODE_VAR,
    OPCODE_LOC,
    OPCODE_INCR,
    OPCODE_ALLC,
    
    OPCODE_ADD,
    OPCODE_SUB,
    OPCODE_MUL,
    OPCODE_DIV,
    OPCODE_MOD,
    
    OPCODE_EQ,
    OPCODE_NE,
    OPCODE_GT,
    OPCODE_LT,
    
    OPCODE_AND,
    OPCODE_OR,
    OPCODE_NOT,
    
    OPCODE_JMP,
    OPCODE_JC,
    OPCODE_LBL,
    
    OPCODE_EXIT,
    OPCODE_OPEN,
    OPCODE_CLOSE,
    OPCODE_READ,
    OPCODE_WRITE,
    
    OPCODE_SRC,

};

defOp(nop);

defOp(mov);
defOp(cpy);
defOp(var);
defOp(loc);
defOp(incr);
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

defOp(exit_);
defOp(open_);
defOp(close_);
defOp(read_);
defOp(write_);

defOp(src);

#endif

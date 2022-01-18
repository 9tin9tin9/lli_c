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
    OPCODE_ALLC,
    OPCODE_PUSH,
    OPCODE_POP,
    OPCODE_LTOF,
    OPCODE_FTOL,
    
    OPCODE_ADD,
    OPCODE_SUB,
    OPCODE_MUL,
    OPCODE_DIV,
    OPCODE_MOD,
    OPCODE_INC,
    OPCODE_DEC,
    OPCODE_ADDF,
    OPCODE_SUBF,
    OPCODE_MULF,
    OPCODE_DIVF,
    OPCODE_INCF,
    OPCODE_DECF,
    
    OPCODE_EQ,
    OPCODE_NE,
    OPCODE_GT,
    OPCODE_LT,
    OPCODE_EQF,
    OPCODE_NEF,
    OPCODE_GTF,
    OPCODE_LTF,
    
    OPCODE_AND,
    OPCODE_OR,
    OPCODE_NOT,
    
    OPCODE_JMP,
    OPCODE_JC,
    OPCODE_LBL,
    OPCODE_CALL,
    OPCODE_RET,
    
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
defOp(allc);
defOp(push);
defOp(pop);
defOp(ltof);
defOp(ftol);

defOp(add);
defOp(sub);
defOp(mul);
defOp(div_);
defOp(mod);
defOp(inc);
defOp(dec);
defOp(addf);
defOp(subf);
defOp(mulf);
defOp(divf);
defOp(incf);
defOp(decf);

defOp(eq);
defOp(ne);
defOp(gt);
defOp(lt);
defOp(eqf);
defOp(nef);
defOp(gtf);
defOp(ltf);

defOp(and);
defOp(or);
defOp(not);

defOp(jmp);
defOp(jc);
defOp(lbl);
defOp(call);
defOp(ret);

defOp(exit_);
defOp(open_);
defOp(close_);
defOp(read_);
defOp(write_);

defOp(src);

#endif

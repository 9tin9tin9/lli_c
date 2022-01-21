#ifndef _OPDEF_H_
#define _OPDEF_H_

#include "op.h"

Error run(Mem* m, Code* c);

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
    OPCODE_PRINT_NUM,
    OPCODE_HALT,

};

#define addEntry(op) { \
    Hashmap_insert(&opIdxTable, #op, i++); \
}

static inline void
op_initOpTable()
{
    int i = 0;
    addEntry(nop);

    addEntry(mov);
    addEntry(cpy);
    addEntry(var);
    addEntry(loc);
    addEntry(allc);
    addEntry(push);
    addEntry(pop);
    addEntry(ltof);
    addEntry(ftol);

    addEntry(add);
    addEntry(sub);
    addEntry(mul);
    addEntry(div);
    addEntry(mod);
    addEntry(inc);
    addEntry(dec);
    addEntry(addf);
    addEntry(subf);
    addEntry(mulf);
    addEntry(divf);
    addEntry(incf);
    addEntry(decf);

    addEntry(eq);
    addEntry(ne);
    addEntry(gt);
    addEntry(lt);
    addEntry(eqf);
    addEntry(nef);
    addEntry(gtf);
    addEntry(ltf);

    addEntry(and);
    addEntry(or);
    addEntry(not);

    addEntry(jmp);
    addEntry(jc);
    addEntry(lbl);
    addEntry(call);
    addEntry(ret);

    addEntry(exit);
    addEntry(open);
    addEntry(close);
    addEntry(read);
    addEntry(write);

    addEntry(src);
    addEntry(print_num);
    addEntry(OPCODE_HALT);
}

#undef addEntry

#define p(op_) case op_: return #op_;

static inline const char*
OpCodeStr(enum Opcode op)
{
    switch (op){
        p(OPCODE_NOP)
        p(OPCODE_MOV)
        p(OPCODE_CPY)
        p(OPCODE_VAR)
        p(OPCODE_LOC)
        p(OPCODE_ALLC)
        p(OPCODE_PUSH)
        p(OPCODE_POP)
        p(OPCODE_LTOF)
        p(OPCODE_FTOL)
        p(OPCODE_ADD)
        p(OPCODE_SUB)
        p(OPCODE_MUL)
        p(OPCODE_DIV)
        p(OPCODE_MOD)
        p(OPCODE_INC)
        p(OPCODE_DEC)
        p(OPCODE_ADDF)
        p(OPCODE_SUBF)
        p(OPCODE_MULF)
        p(OPCODE_DIVF)
        p(OPCODE_INCF)
        p(OPCODE_DECF)
        p(OPCODE_EQ)
        p(OPCODE_NE)
        p(OPCODE_GT)
        p(OPCODE_LT)
        p(OPCODE_EQF)
        p(OPCODE_NEF)
        p(OPCODE_GTF)
        p(OPCODE_LTF)
        p(OPCODE_AND)
        p(OPCODE_OR)
        p(OPCODE_NOT)
        p(OPCODE_JMP)
        p(OPCODE_JC)
        p(OPCODE_LBL)
        p(OPCODE_CALL)
        p(OPCODE_RET)
        p(OPCODE_EXIT)
        p(OPCODE_OPEN)
        p(OPCODE_CLOSE)
        p(OPCODE_READ)
        p(OPCODE_WRITE)
        p(OPCODE_SRC)
        p(OPCODE_PRINT_NUM)
        p(OPCODE_HALT)
    }
}

#undef p

#endif

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

#define addEntry(op) { \
    defOp(op); \
    Vec_push(&funcVec, &op); \
    Hashmap_insert(&opIdxTable, #op, Vec_count(&funcVec)-1); \
}

#define addEntryAlternate(op) { \
    defOp(op##_); \
    Vec_push(&funcVec, &op##_); \
    Hashmap_insert(&opIdxTable, #op, Vec_count(&funcVec)-1); \
}

static inline void
op_initOpTable()
{
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
    addEntryAlternate(div);
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

    addEntryAlternate(exit);
    addEntryAlternate(open);
    addEntryAlternate(close);
    addEntryAlternate(read);
    addEntryAlternate(write);

    addEntry(src);

    addEntry(print_num);
}

#undef addEntry

#undef defOp

#endif

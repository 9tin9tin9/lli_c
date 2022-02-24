#include "include/opdef.h"
#include "include/sys.h"
#include <stdio.h>

#define NEXT_INTR(line) l = Code_at(c, line); v = &l->toks;
#define SET_LINE_ADDR(line) *Vec_at_unsafe(&m->mem, 1, Value) = Value(Long, line);

#if COMPUTED_GOTO
    #define GOTO(op) NEXT_INTR(Code_ptr(c)); goto *jumpTable[op];
    #define TARGET(op) op: SET_LINE_ADDR(Code_ptr(c));
    #define DISPATCH() NEXT_INTR(Code_ptr(c)++); goto *jumpTable[l->opcode];
#else
    #define GOTO(op) NEXT_INTR(Code_ptr(c)); switch (op)
    #define TARGET(op) case op: SET_LINE_ADDR(Code_ptr(c));
    #define DISPATCH() NEXT_INTR(Code_ptr(c)++); break;
#endif

#define _DEF_OPCODE_MOV(t0, t1) \
            TARGET(OPCODE_MOV_##t0##t1) \
            { \
                Value val; \
                try(Tok_getValue(t1, Vec_at_unsafe(v, 1, Tok), m, &val)); \
                try(Tok_writeValue(t0, Vec_at_unsafe(v, 0, Tok), m, &val)); \
                DISPATCH() \
            }
#define DEF_OPCODE_MOV(s, product, d) _DEF_OPCODE_MOV CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_CPY(t0, t1, t2) \
            TARGET(OPCODE_CPY_##t0##t1##t2) \
            { \
                long des; \
                try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &des)); \
                long src; \
                try(Tok_getLoc(t1, Vec_at_unsafe(v, 1, Tok), m, &src)); \
                Value size; \
                try(Tok_getValue(t2, Vec_at_unsafe(v, 2, Tok), m, &size)); \
                \
                Value val; \
                for (size_t i = 0; i < size.Long; i++){ \
                    try(Mem_mem_at(m, src, &val)); \
                    try(Tok_writeValue(Idx, &Tok(Idx, des), m, &val)); \
                    idxIncr(&des, 1); \
                    idxIncr(&src, 1); \
                } \
                DISPATCH() \
            }
#define DEF_OPCODE_CPY(s, product, d) _DEF_OPCODE_CPY CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_VAR(t0, t1) \
            TARGET(OPCODE_VAR_##t0##t1) \
            { \
                HashIdx var; \
                long idx; \
                try(Tok_getSym(Vec_at_unsafe(v, 0, Tok), &var)); \
                try(Tok_getLoc(t1, Vec_at_unsafe(v, 1, Tok), m, &idx)); \
                Mem_var_set(m, var.idx, idx); \
                DISPATCH() \
            }
#define DEF_OPCODE_VAR(s, product, d) _DEF_OPCODE_VAR CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_LOC(t0) \
            TARGET(OPCODE_LOC_##t0) \
            { \
                long i; \
                try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &i)); \
                *Vec_at_unsafe(&m->mem, 0, Value) = Value(Long, i); \
                DISPATCH() \
            }
#define DEF_OPCODE_LOC(s, product, d) _DEF_OPCODE_LOC CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_ALLC(t0) \
            TARGET(OPCODE_ALLC_##t0) \
            { \
                Tok* sizeTok = Vec_at_unsafe(v, 0, Tok); \
                Value sizeS; \
                try(Tok_getValue(t0, sizeTok, m, &sizeS)); \
                for (int i = 0; i < sizeS.Long; i++){ \
                    Mem_mem_push(m, 1); \
                } \
                DISPATCH() \
            }
#define DEF_OPCODE_ALLC(s, product, d) _DEF_OPCODE_ALLC CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))
            
#define _DEF_OPCODE_PUSH(t0, t1) \
            TARGET(OPCODE_PUSH_##t0##t1) \
            { \
                long idx; \
                Value* ptr; \
                Value val; \
                /* getValue */ \
                try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &idx)); \
                if (idx < 0) return Error_CannotWriteToNMem; \
                ptr = Vec_at_unsafe(&m->mem, idx, Value); \
                if (ptr->Long < 0) return Error_CannotWriteToNMem; \
                /* writeValue */\
                ptr->Long++; \
                try(Tok_getValue(t1, Vec_at_unsafe(v, 1, Tok), m, &val)); \
                try(Mem_mem_set(m, ptr->Long, &val)); \
                DISPATCH() \
            }
#define DEF_OPCODE_PUSH(s, product, d) _DEF_OPCODE_PUSH CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_POP(t0) \
            TARGET(OPCODE_POP_##t0) \
            { \
                long idx; \
                Value* ptr; \
                Value val; \
                /* getUint */\
                try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &idx)); \
                if (idx < 0) return Error_CannotWriteToNMem; \
                ptr = Vec_at_unsafe(&m->mem, idx, Value); \
                if (ptr->Long < 0) return Error_CannotWriteToNMem; \
                try(Mem_mem_at(m, ptr->Long, &val)); \
                *Vec_at_unsafe(&m->mem, 0, Value) = val; \
                /* writeValue */\
                ptr->Long--; \
                DISPATCH() \
            }
#define DEF_OPCODE_POP(s, product, d) _DEF_OPCODE_POP CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_LTOF(t0) \
            TARGET(OPCODE_LTOF_##t0) \
            { \
                long idx; \
                try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &idx)); \
                if (idx < 0) return Error_CannotWriteToNMem; \
                Value* val = Vec_at_unsafe(&m->mem, idx, Value); \
                if (val->type == 'L'){ \
                    val->Double = val->Long; \
                } \
                DISPATCH() \
            }
#define DEF_OPCODE_LTOF(s, product, d) _DEF_OPCODE_LTOF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_FTOL(t0) \
            TARGET(OPCODE_FTOL_##t0) \
            { \
                long idx; \
                try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &idx)); \
                if (idx < 0) return Error_CannotWriteToNMem; \
                Value* val = Vec_at_unsafe(&m->mem, idx, Value); \
                if (val->type == 'D'){ \
                    val->Long = val->Double; \
                } \
                DISPATCH() \
            }
#define DEF_OPCODE_FTOL(s, product, d) _DEF_OPCODE_FTOL CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define math_parseArg(v, m, t0, t1, l, r) \
    try(Tok_getValue(t0, Vec_at_unsafe(v, 0, Tok), m, l)); \
    try(Tok_getValue(t1, Vec_at_unsafe(v, 1, Tok), m, r));

#define math(op_, v_, m_, t0, t1) \
    Value left, right; \
    math_parseArg(v_, m_, t0, t1, &left, &right); \
    long result = left.Long op_ right.Long; \
    *Vec_at_unsafe(&m_->mem, 0, Value) = Value(Long, result); \

#define incrDecr(op, v, m, t0) \
    Value val; \
    long loc; \
    try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &loc)); \
    try(Mem_mem_at(m, loc, &val)); \
    val.Long op; \
    try(Mem_mem_set(m, loc, &val)); \

#define mathf(op_, v_, m_, t0, t1) \
    Value left, right; \
    math_parseArg(v_, m_, t0, t1, &left, &right); \
    if (left.type == 'L') left.Double = left.Long; \
    if (right.type == 'L') right.Double = right.Long; \
    double result = left.Double op_ right.Double; \
    *Vec_at_unsafe(&m_->mem, 0, Value) = Value(Double, result); \

#define incrDecrf(op, v, m, t0) \
    Value val; \
    long loc; \
    try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &loc)); \
    try(Mem_mem_at(m, loc, &val)); \
    val.Double op; \
    try(Mem_mem_set(m, loc, &val)); \

#define _DEF_OPCODE_ADD(t0, t1) \
            TARGET(OPCODE_ADD_##t0##t1) \
            { \
                math(+, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_ADD(s, product, d) _DEF_OPCODE_ADD CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_SUB(t0, t1) \
            TARGET(OPCODE_SUB_##t0##t1) \
            { \
                math(-, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_SUB(s, product, d) _DEF_OPCODE_SUB CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_MUL(t0, t1) \
            TARGET(OPCODE_MUL_##t0##t1) \
            { \
                math(*, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_MUL(s, product, d) _DEF_OPCODE_MUL CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_DIV(t0, t1) \
            TARGET(OPCODE_DIV_##t0##t1) \
            { \
                math(*, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_DIV(s, product, d) _DEF_OPCODE_DIV CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_MOD(t0, t1) \
            TARGET(OPCODE_MOD_##t0##t1) \
            { \
                math(%, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_MOD(s, product, d) _DEF_OPCODE_MOD CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_INC(t0) \
            TARGET(OPCODE_INC_##t0) \
            { \
                incrDecr(++, v, m, t0); \
                DISPATCH() \
            }
#define DEF_OPCODE_INC(s, product, d) _DEF_OPCODE_INC CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_DEC(t0) \
            TARGET(OPCODE_DEC_##t0) \
            { \
                incrDecr(--, v, m, t0); \
                DISPATCH() \
            }
#define DEF_OPCODE_DEC(s, product, d) _DEF_OPCODE_DEC CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_ADDF(t0, t1) \
            TARGET(OPCODE_ADDF_##t0##t1) \
            { \
                mathf(+, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_ADDF(s, product, d) _DEF_OPCODE_ADDF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_SUBF(t0, t1) \
            TARGET(OPCODE_SUBF_##t0##t1) \
            { \
                mathf(-, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_SUBF(s, product, d) _DEF_OPCODE_SUBF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_MULF(t0, t1) \
            TARGET(OPCODE_MULF_##t0##t1) \
            { \
                mathf(*, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_MULF(s, product, d) _DEF_OPCODE_MULF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_DIVF(t0, t1) \
            TARGET(OPCODE_DIVF_##t0##t1) \
            { \
                mathf(*, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_DIVF(s, product, d) _DEF_OPCODE_DIVF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_INCF(t0) \
            TARGET(OPCODE_INCF_##t0) \
            { \
                incrDecrf(++, v, m, t0); \
                DISPATCH() \
            }
#define DEF_OPCODE_INCF(s, product, d) _DEF_OPCODE_INCF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_DECF(t0) \
            TARGET(OPCODE_DECF_##t0) \
            { \
                incrDecrf(--, v, m, t0); \
                DISPATCH() \
            }
#define DEF_OPCODE_DECF(s, product, d) _DEF_OPCODE_DECF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_EQ(t0, t1) \
            TARGET(OPCODE_EQ_##t0##t1) \
            { \
                math(==, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_EQ(s, product, d) _DEF_OPCODE_EQ CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_NE(t0, t1) \
            TARGET(OPCODE_NE_##t0##t1) \
            { \
                math(!=, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_NE(s, product, d) _DEF_OPCODE_NE CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_GT(t0, t1) \
            TARGET(OPCODE_GT_##t0##t1) \
            { \
                math(>, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_GT(s, product, d) _DEF_OPCODE_GT CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_LT(t0, t1) \
            TARGET(OPCODE_LT_##t0##t1) \
            { \
                math(<, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_LT(s, product, d) _DEF_OPCODE_LT CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_EQF(t0, t1) \
            TARGET(OPCODE_EQF_##t0##t1) \
            { \
                mathf(==, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_EQF(s, product, d) _DEF_OPCODE_EQF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_NEF(t0, t1) \
            TARGET(OPCODE_NEF_##t0##t1) \
            { \
                mathf(!=, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_NEF(s, product, d) _DEF_OPCODE_NEF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_GTF(t0, t1) \
            TARGET(OPCODE_GTF_##t0##t1) \
            { \
                mathf(>, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_GTF(s, product, d) _DEF_OPCODE_GTF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_LTF(t0, t1) \
            TARGET(OPCODE_LTF_##t0##t1) \
            { \
                mathf(<, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_LTF(s, product, d) _DEF_OPCODE_LTF CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_AND(t0, t1) \
            TARGET(OPCODE_AND_##t0##t1) \
            { \
                math(&&, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_AND(s, product, d) _DEF_OPCODE_AND CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_OR(t0, t1) \
            TARGET(OPCODE_OR_##t0##t1) \
            { \
                math(||, v, m, t0, t1); \
                DISPATCH() \
            }
#define DEF_OPCODE_OR(s, product, d) _DEF_OPCODE_OR CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_NOT(t0)  \
            TARGET(OPCODE_NOT_##t0) \
            { \
                Value value; \
                try(Tok_getValue(t0, Vec_at_unsafe(v, 0, Tok), m, &value)); \
                long result = value.Long == 0; \
                *Vec_at_unsafe(&m->mem, 0, Value) = Value(Long, result); \
                DISPATCH() \
            }
#define DEF_OPCODE_NOT(s, product, d) _DEF_OPCODE_NOT CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_JMP(t0) \
            TARGET(OPCODE_JMP_##t0) \
            { \
                Value loc; \
                try(Tok_getValue(t0, Vec_at_unsafe(v, 0, Tok), m, &loc)); \
                Code_ptr_set(c, loc.Long); \
                DISPATCH() \
            }
#define DEF_OPCODE_JMP(s, product, d) _DEF_OPCODE_JMP CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_JC(t0, t1) \
            TARGET(OPCODE_JC_##t0##t1) \
            { \
                Value cond; \
                try(Tok_getValue(t0, Vec_at_unsafe(v, 0, Tok), m, &cond)); \
                if (cond.Long){ \
                    Value loc; \
                    try(Tok_getValue(t1, Vec_at_unsafe(v, 1, Tok), m, &loc)); \
                    Code_ptr_set(c, loc.Long); \
                } \
                DISPATCH() \
            }
#define DEF_OPCODE_JC(s, product, d) _DEF_OPCODE_JC CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_LBL(t0) \
            TARGET(OPCODE_LBL_##t0) \
                DISPATCH()
#define DEF_OPCODE_LBL(s, product, d) _DEF_OPCODE_LBL CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_CALL(t0, t1) \
            TARGET(OPCODE_CALL_##t0##t1) \
            { \
                long idx; \
                Value* ptr; \
                Value curr; \
                Value loc; \
                \
                /* getUint */ \
                try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &idx)); \
                if (idx < 0) return Error_CannotWriteToNMem; \
                ptr = Vec_at_unsafe(&m->mem, idx, Value); \
                if (ptr->Long < 0) return Error_CannotWriteToNMem; \
                \
                /* writeValue */ \
                ptr->Long++; \
                \
                try(Mem_mem_at(m, -1, &curr)); \
                curr.Long++; \
                try(Mem_mem_set(m, ptr->Long, &curr)); \
                try(Tok_getValue(t1, Vec_at_unsafe(v, 1, Tok), m, &loc)); \
                Code_ptr_set(c, loc.Long); \
                DISPATCH() \
            }
#define DEF_OPCODE_CALL(s, product, d) _DEF_OPCODE_CALL CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_RET(t0) \
            TARGET(OPCODE_RET_##t0) \
            { \
                Value* ptr; \
                Value loc; \
                long idx; \
                \
                /* getUint */ \
                try(Tok_getLoc(t0, Vec_at_unsafe(v, 0, Tok), m, &idx)); \
                if (idx < 0) return Error_CannotWriteToNMem; \
                ptr = Vec_at_unsafe(&m->mem, idx, Value); \
                if (ptr->type != 'L') return Error_NotInteger; \
                \
                try(Mem_mem_at(m, ptr->Long, &loc)); \
                \
                /* writeValue */ \
                ptr->Long--; \
                \
                Code_ptr_set(c, loc.Long); \
                DISPATCH() \
            }
#define DEF_OPCODE_RET(s, product, d) _DEF_OPCODE_RET CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_SYS(t0) \
            TARGET(OPCODE_SYS_##t0) \
            { \
                Value code; \
                try(Tok_getValue(t0, Vec_at_unsafe(v, 0, Tok), m, &code)); \
                switch (code.Long) { \
                    case SYSCALLCODE_EXIT: \
                        try(sys_exit(m)); \
                        break; \
                    case SYSCALLCODE_READ: \
                        try(sys_read(m)); \
                        break; \
                    case SYSCALLCODE_WRITE: \
                        try(sys_write(m)); \
                        break; \
                    case SYSCALLCODE_OPEN: \
                        try(sys_open(m)); \
                        break; \
                    case SYSCALLCODE_CLOSE: \
                        try(sys_close(m)); \
                        break; \
                } \
                DISPATCH(); \
            }
#define DEF_OPCODE_SYS(s, product, d) _DEF_OPCODE_SYS CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

#define _DEF_OPCODE_PRINT_NUM(t0, t1) \
            TARGET(OPCODE_PRINT_NUM_##t0##t1) \
            { \
                Value fd; \
                Value value; \
                try(Tok_getValue(t0, Vec_at(v, 0, Tok), m, &fd)); \
                try(Tok_getValue(t1, Vec_at(v, 1, Tok), m, &value)); \
                if (value.type == 'D'){ \
                    fprintf(fdopen(fd.Long, "w"), "%f\n", value.Double); \
                }else{ \
                    fprintf(fdopen(fd.Long, "w"), "%ld\n", value.Long); \
                } \
                DISPATCH() \
            }
#define DEF_OPCODE_PRINT_NUM(s, product, d) _DEF_OPCODE_PRINT_NUM CHAOS_PP_SEQ_TO_TUPLE(CHAOS_PP_SEQ_TAIL(product))

Error
run(Mem* m, Code* c)
{

#if COMPUTED_GOTO
#include "include/jumpTable.h"
#endif

    const Line* l;
    const Vec* v;
    while(Code_ptr(c) < Code_len(c)){

        GOTO(l->opcode)
        {
            TARGET(OPCODE_HALT_)
            {
                *Vec_at_unsafe(&m->mem, 0, Value) = Value(Long, 0);
                goto end;
            }

            TARGET(OPCODE_NOP_)
            {
                DISPATCH()
            }

            GENERATE(DEF_OPCODE_MOV, OPCODE_MOV, OPCODE_MOV_ARG())
            GENERATE(DEF_OPCODE_CPY, OPCODE_CPY, OPCODE_CPY_ARG())
            TARGET(OPCODE_SIZE_)
            {
                *Vec_at_unsafe(&m->mem, 0, Value) =
                    Value(Long, Mem_mem_len(m));
                DISPATCH()
            }
            GENERATE(DEF_OPCODE_VAR, OPCODE_VAR, OPCODE_VAR_ARG())
            GENERATE(DEF_OPCODE_LOC, OPCODE_LOC, OPCODE_LOC_ARG())
            GENERATE(DEF_OPCODE_ALLC, OPCODE_ALLC, OPCODE_ALLC_ARG())
            GENERATE(DEF_OPCODE_PUSH, OPCODE_PUSH, OPCODE_PUSH_ARG())
            GENERATE(DEF_OPCODE_POP, OPCODE_POP, OPCODE_POP_ARG())
            GENERATE(DEF_OPCODE_LTOF, OPCODE_LTOF, OPCODE_LTOF_ARG())
            GENERATE(DEF_OPCODE_FTOL, OPCODE_FTOL, OPCODE_FTOL_ARG())

            GENERATE(DEF_OPCODE_ADD, OPCODE_ADD, OPCODE_ADD_ARG())
            GENERATE(DEF_OPCODE_SUB, OPCODE_SUB, OPCODE_SUB_ARG())
            GENERATE(DEF_OPCODE_MUL, OPCODE_MUL, OPCODE_MUL_ARG())
            GENERATE(DEF_OPCODE_DIV, OPCODE_DIV, OPCODE_DIV_ARG())
            GENERATE(DEF_OPCODE_MOD, OPCODE_MOD, OPCODE_MOD_ARG())
            GENERATE(DEF_OPCODE_INC, OPCODE_INC, OPCODE_INC_ARG())
            GENERATE(DEF_OPCODE_DEC, OPCODE_DEC, OPCODE_DEC_ARG())

            GENERATE(DEF_OPCODE_ADDF, OPCODE_ADDF, OPCODE_ADDF_ARG())
            GENERATE(DEF_OPCODE_SUBF, OPCODE_SUBF, OPCODE_SUBF_ARG())
            GENERATE(DEF_OPCODE_MULF, OPCODE_MULF, OPCODE_MULF_ARG())
            GENERATE(DEF_OPCODE_DIVF, OPCODE_DIVF, OPCODE_DIVF_ARG())
            GENERATE(DEF_OPCODE_INCF, OPCODE_INCF, OPCODE_INCF_ARG())
            GENERATE(DEF_OPCODE_DECF, OPCODE_DECF, OPCODE_DECF_ARG())

            GENERATE(DEF_OPCODE_EQ, OPCODE_EQ, OPCODE_EQ_ARG())
            GENERATE(DEF_OPCODE_NE, OPCODE_NE, OPCODE_NE_ARG())
            GENERATE(DEF_OPCODE_GT, OPCODE_GT, OPCODE_GT_ARG())
            GENERATE(DEF_OPCODE_LT, OPCODE_LT, OPCODE_LT_ARG())

            GENERATE(DEF_OPCODE_EQF, OPCODE_EQF, OPCODE_EQF_ARG())
            GENERATE(DEF_OPCODE_NEF, OPCODE_NEF, OPCODE_NEF_ARG())
            GENERATE(DEF_OPCODE_GTF, OPCODE_GTF, OPCODE_GTF_ARG())
            GENERATE(DEF_OPCODE_LTF, OPCODE_LTF, OPCODE_LTF_ARG())

            GENERATE(DEF_OPCODE_AND, OPCODE_AND, OPCODE_AND_ARG())
            GENERATE(DEF_OPCODE_OR, OPCODE_OR, OPCODE_OR_ARG())
            GENERATE(DEF_OPCODE_NOT, OPCODE_NOT, OPCODE_NOT_ARG())

            GENERATE(DEF_OPCODE_JMP, OPCODE_JMP, OPCODE_JMP_ARG())
            GENERATE(DEF_OPCODE_JC, OPCODE_JC, OPCODE_JC_ARG())
            GENERATE(DEF_OPCODE_LBL, OPCODE_LBL, OPCODE_LBL_ARG())
            GENERATE(DEF_OPCODE_CALL, OPCODE_CALL, OPCODE_CALL_ARG())
            GENERATE(DEF_OPCODE_RET, OPCODE_RET, OPCODE_RET_ARG())
            GENERATE(DEF_OPCODE_SYS, OPCODE_SYS, OPCODE_SYS_ARG())

            TARGET(OPCODE_SRC_Sym)
                DISPATCH()

            GENERATE(DEF_OPCODE_PRINT_NUM, OPCODE_PRINT_NUM, OPCODE_PRINT_NUM_ARG())
        }
    }
end:
    return Ok;
}

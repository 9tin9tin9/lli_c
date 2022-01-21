#include "include/opdef.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

// #define COMPUTED_GOTO

Error
math_parseArg(const Vec* v, Mem* m, Value* left, Value* right)
{
    argcGuard(v, 2);
    try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, left));
    try(Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, right));
    return Ok;
}

#define math(op_, v_, m_) \
    Value left, right; \
    try(math_parseArg(v_, m_, &left, &right)); \
    long result = left.Long op_ right.Long; \
    *Vec_at_unsafe(&m_->mem, 0, Value) = Value(Long, result); \

#define incrDecr(op, v, m) \
    argcGuard(v, 1); \
    Value val; \
    long loc; \
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &loc)); \
    try(Mem_mem_at(m, loc, &val)); \
    val.Long op; \
    try(Mem_mem_set(m, loc, &val)); \

#define mathf(op_, v_, m_) \
    Value left, right; \
    try(math_parseArg(v_, m_, &left, &right)); \
    if (left.type == 'L') left.Double = left.Long; \
    if (right.type == 'L') right.Double = right.Long; \
    double result = left.Double op_ right.Double; \
    *Vec_at_unsafe(&m_->mem, 0, Value) = Value(Double, result); \

#define incrDecrf(op, v, m) \
    argcGuard(v, 1); \
    Value val; \
    long loc; \
    try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &loc)); \
    try(Mem_mem_at(m, loc, &val)); \
    val.Double op; \
    try(Mem_mem_set(m, loc, &val)); \

#define cmpf(op_, v_, m_) \
    Value left, right; \
    try(cmp_parseArg(v_, m_, &left, &right)); \
    if (left.type == 'L') left.Double = left.Long; \
    if (right.type == 'L') right.Double = right.Long; \
    double result = left.Double op_ right.Double; \
    *Vec_at_unsafe(&m_->mem, 0, Value) = Value(Double, result); \

Error
cmp_parseArg(const Vec* v, Mem* m, Value* left, Value* right)
{
    argcGuard(v, 2);
    try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, left));
    return Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, right);
}

#define cmp(op_, v_, m_) \
    Value left, right; \
    try(cmp_parseArg(v_, m_, &left, &right)); \
    long result = left.Long op_ right.Long; \
    *Vec_at_unsafe(&m_->mem, 0, Value) = Value(Long, result); \

enum OpenOptions {
    OO_READ = 0, OO_WRITE, OO_APPEND,
    OO_TRUNCATE, OO_CREATE, OO_CREATE_NEW
};

Error
parseOpenOption(size_t oVal, int* oflag)
{
    char options[6] = {0};
    for (int i = 0; i < 6; i++){
        char o = oVal%10;
        if (o > 1){
            return Error_InvalidOpenOption;
        }
        options[i] = o;
        oVal /= 10;
    }
    const char wr = options[OO_READ] + options[OO_WRITE];
    switch (wr){
        case 0:
            return Error_InvalidOpenOption;
        case 1:
            *oflag = options[OO_READ] ? O_RDONLY : O_WRONLY; break;
        case 2:
            *oflag = O_RDWR; break;
    }
    if (options[OO_APPEND]) *oflag |= O_APPEND;
    if (options[OO_TRUNCATE]) *oflag |= O_TRUNC;
    if (options[OO_CREATE]) *oflag |= O_CREAT;
    if (options[OO_CREATE_NEW]) *oflag |= O_CREAT | O_EXCL;
    return Ok;
}

#define NEXT_INTR(line) l = Code_at(c, line); v = &l->toks;

#ifdef COMPUTED_GOTO
    #define GOTO(op) goto *jumpTable[op];
    #define TARGET(op) op:
    #define DISPATCH() NEXT_INTR(++Code_ptr(c)); goto *jumpTable[l->opcode];
#else
    #define GOTO(op) switch (op)
    #define TARGET(op) case op:
    #define DISPATCH() NEXT_INTR(++Code_ptr(c)); break;
#endif

Error
run(Mem* m, Code* c)
{

#ifdef COMPUTED_GOTO
#include "include/jumpTable.h"
#endif

    const Line* l = Code_curr(c);
    const Vec* v = &l->toks;
    while(Code_ptr(c) < Code_len(c)){
        *Vec_at_unsafe(&m->mem, 1, Value) = Value(Long, Code_ptr(c));
        l = Code_curr(c);
        v = &l->toks;

        GOTO(l->opcode)
        {
            TARGET(OPCODE_HALT)
            {
                goto end;
            }

            TARGET(OPCODE_NOP)
            {
                DISPATCH()
            }

            TARGET(OPCODE_MOV)
            {
                argcGuard(v, 2);
                Value val;
                try(Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, &val));
                try(Tok_writeValue(Vec_at_unsafe(v, 0, Tok), m, &val));
                DISPATCH()
            }

            TARGET(OPCODE_CPY)
            {
                argcGuard(v, 3);
                long des;
                try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &des));
                long src;
                try(Tok_getLoc(Vec_at_unsafe(v, 1, Tok), m, &src));
                size_t size;
                try(Tok_getUint(Vec_at_unsafe(v, 2, Tok), m, &size));

                Value val;
                for (size_t i = 0; i < size; i++){
                    try(Mem_mem_at(m, src, &val));
                    try(Tok_writeValue(&Tok(Idx, des), m, &val));
                    idxIncr(&des, 1);
                    idxIncr(&src, 1);
                }
                DISPATCH()
            }

            TARGET(OPCODE_VAR)
            {
                argcGuard(v, 2);
                HashIdx var;
                long idx;
                try(Tok_getSym(Vec_at_unsafe(v, 0, Tok), &var));
                try(Tok_getLoc(Vec_at_unsafe(v, 1, Tok), m, &idx));
                Mem_var_set(m, var.idx, idx);
                DISPATCH()
            }

            TARGET(OPCODE_LOC)
            {
                argcGuard(v, 1);
                long i;
                try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &i));
                *Vec_at_unsafe(&m->mem, 0, Value) = Value(Long, i);
                DISPATCH()
            }

            TARGET(OPCODE_ALLC)
            {
                argcGuard(v, 1);
                Tok* sizeTok = Vec_at_unsafe(v, 0, Tok);
                size_t sizeS;
                try(Tok_getUint(sizeTok, m, &sizeS));
                for (int i = 0; i < sizeS; i++){
                    Mem_mem_push(m, 1);
                }
                DISPATCH()
            }
            
            TARGET(OPCODE_PUSH)
            {
                argcGuard(v, 2);
                long idx;
                Value* ptr;
                Value val;

                // getUint
                try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
                if (idx < 0) return Error_CannotWriteToNMem;
                ptr = Vec_at_unsafe(&m->mem, idx, Value);
                if (ptr->type != 'L') return Error_NotInteger;
                if (ptr->Long < 0) return Error_CannotWriteToNMem;

                // writeValue
                ptr->Long++;

                try(Tok_getValue(Vec_at_unsafe(v, 1, Tok), m, &val));
                try(Mem_mem_set(m, ptr->Long, &val));
                DISPATCH()
            }

            TARGET(OPCODE_POP)
            {
                argcGuard(v, 1);
                long idx;
                Value* ptr;
                Value val;

                // getUint
                try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
                if (idx < 0) return Error_CannotWriteToNMem;
                ptr = Vec_at_unsafe(&m->mem, idx, Value);
                if (ptr->type != 'L') return Error_NotInteger;

                try(Mem_mem_at(m, ptr->Long, &val));
                *Vec_at_unsafe(&m->mem, 0, Value) = val;

                // writeValue
                ptr->Long--;
                DISPATCH()
            }
            
            TARGET(OPCODE_LTOF)
            {
                argcGuard(v, 1);
                long idx;
                try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
                if (idx < 0) return Error_CannotWriteToNMem;
                Value* val = Vec_at_unsafe(&m->mem, idx, Value);
                if (val->type == 'L'){
                    val->Double = val->Long;
                }
                DISPATCH()
            }

            TARGET(OPCODE_FTOL)
            {
                argcGuard(v, 1);
                long idx;
                try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
                if (idx < 0) return Error_CannotWriteToNMem;
                Value* val = Vec_at_unsafe(&m->mem, idx, Value);
                if (val->type == 'D'){
                    val->Long = val->Double;
                }
                DISPATCH()
            }

            TARGET(OPCODE_ADD)
            {
                math(+, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_SUB)
            {
                math(-, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_MUL)
            {
                math(*, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_DIV)
            {
                math(/, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_MOD)
            {
                Value left, right;
                try(math_parseArg(v, m, &left, &right));
                long result = left.Long % right.Long;
                Mem_mem_set(m, 0, &Value(Long, result));
                DISPATCH()
            }

            TARGET(OPCODE_INC)
            {
                incrDecr(++, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_DEC)
            {
                incrDecr(--, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_ADDF)
            {
                mathf(+, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_SUBF)
            {
                mathf(-, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_MULF)
            {
                mathf(*, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_DIVF)
            {
                mathf(/, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_INCF)
            {
                incrDecrf(++, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_DECF)
            {
                incrDecrf(--, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_EQ)
            {
                cmp(==, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_NE)
            {
                cmp(!=, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_GT)
            {
                cmp(>, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_LT)
            {
                cmp(<, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_EQF)
            {
                cmpf(==, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_NEF)
            {
                cmpf(!=, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_GTF)
            {
                cmpf(>, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_LTF)
            {
                cmpf(<, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_AND)
            {
                math(&&, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_OR)
            {
                math(||, v, m);
                DISPATCH()
            }

            TARGET(OPCODE_NOT)
            {
                argcGuard(v, 1);
                Value value;
                try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, &value));
                long result = value.Long == 0;
                *Vec_at_unsafe(&m->mem, 0, Value) = Value(Long, result);
                DISPATCH()
            }

            TARGET(OPCODE_JMP)
            {
                argcGuard(v, 1);
                size_t loc;
                try(Tok_getUint(Vec_at_unsafe(v, 0, Tok), m, &loc));
                Code_ptr_set(c, loc);
                DISPATCH()
            }

            TARGET(OPCODE_JC)
            {
                argcGuard(v, 2);
                Value cond;
                try(Tok_getValue(Vec_at_unsafe(v, 0, Tok), m, &cond));
                if (cond.Long){
                    size_t loc;
                    try(Tok_getUint(Vec_at_unsafe(v, 1, Tok), m, &loc));
                    Code_ptr_set(c, loc);
                }
                DISPATCH()
            }

            TARGET(OPCODE_LBL)
                DISPATCH()

            TARGET(OPCODE_CALL)
            {
                argcGuard(v, 2);
                long idx;
                Value* ptr;
                Value curr;
                size_t loc;

                // getUint
                try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
                if (idx < 0) return Error_CannotWriteToNMem;
                ptr = Vec_at_unsafe(&m->mem, idx, Value);
                if (ptr->type != 'L') return Error_NotInteger;
                if (ptr->Long < 0) return Error_CannotWriteToNMem;

                // writeValue
                ptr->Long++;

                try(Mem_mem_at(m, -1, &curr));
                curr.Long++;
                try(Mem_mem_set(m, ptr->Long, &curr));
                try(Tok_getUint(Vec_at_unsafe(v, 1, Tok), m, &loc));
                Code_ptr_set(c, loc);
                DISPATCH()
            }

            TARGET(OPCODE_RET)
            {
                argcGuard(v, 1);
                Value* ptr;
                Value loc;
                long idx;

                // getUint
                try(Tok_getLoc(Vec_at_unsafe(v, 0, Tok), m, &idx));
                if (idx < 0) return Error_CannotWriteToNMem;
                ptr = Vec_at_unsafe(&m->mem, idx, Value);
                if (ptr->type != 'L') return Error_NotInteger;

                try(Mem_mem_at(m, ptr->Long, &loc));

                // writeValue
                ptr->Long--;

                Code_ptr_set(c, loc.Long);
                DISPATCH()
            }

            TARGET(OPCODE_EXIT)
            {
                argcGuard(v, 1);
                long exitCode;
                try(Tok_getInt(Vec_at_unsafe(v, 0, Tok), m, &exitCode));
                exit(exitCode);
            }

            TARGET(OPCODE_WRITE)
            {
                argcGuard(v, 3);
                size_t fd;
                try(Tok_getUint(Vec_at_unsafe(v, 0, Tok), m, &fd));
                bool* slot = Vec_at_unsafe(&m->fd, fd, bool);
                if (!slot || !*slot){
                    return Error_BadFileDescriptor;
                }
                FILE* f = fdopen(fd, "w");
                long srcIdx;
                try(Tok_getLoc(Vec_at_unsafe(v, 1, Tok), m, &srcIdx));
                size_t size;
                try(Tok_getUint(Vec_at_unsafe(v, 2, Tok), m, &size));
                size_t i;
                for (i = 0; i < size; i++)
                {
                    Value value;
                    try(Mem_mem_at(m, srcIdx, &value));
                    char ch = value.Long;
                    if (!fwrite(&ch, sizeof(char), 1, f))
                        return Error_IoError;
                    idxIncr(&srcIdx, 1);
                }
                fflush(f);
                Mem_mem_set(m, 0, &Value(Long, i));
                DISPATCH()
            }

            TARGET(OPCODE_READ)
            {
                argcGuard(v, 3);
                size_t fd;
                try(Tok_getUint(Vec_at_unsafe(v, 0, Tok), m, &fd));
                bool* slot = Vec_at_unsafe(&m->fd, fd, bool);
                if (!slot || !*slot){
                    return Error_BadFileDescriptor;
                }
                FILE* f = fdopen(fd, "r");
                long desIdx;
                try(Tok_getLoc(Vec_at_unsafe(v, 1, Tok), m, &desIdx));
                size_t size;
                try(Tok_getUint(Vec_at_unsafe(v, 2, Tok), m, &size));
                char buf[MAX_INPUT];
                size_t readSize = fread(buf, sizeof(char), MAX_INPUT, f);
                if (!readSize){
                    return Error_IoError;
                }
                size_t i;
                for (i = 0; i < readSize && i < size; i++){
                    Value c = Value(Long, buf[i]);
                    try(Mem_mem_set(m, desIdx+i, &c));
                }
                Mem_mem_set(m, 0, &Value(Long, i));
                DISPATCH()
            }

            // Open file and set [0] to fd
            //      open) name(Ptr), option(Value)
            //
            // Open options) number consisting 6 or less digits
            //
            //  _ _ _ _ _ _
            //  6 5 4 3 2 1
            //
            //  1) read
            //  2) write
            //  3) append
            //  4) truncate
            //  5) create
            //  6) create_new
            //
            //  All digits should be either be 0 or 1, representing boolean value.
            //  Boolean values will be passed to std))fs))OpenOptions.
            //  Read rust docs for more details about each option.
            //
            //  Example) opening text.txt in read only mode
            //      open)"text.txt",1
            //
            //  Example) opening text.txt in write-only mode, 
            //           create file if it does not exists,
            //           and will truncate it if it does.
            //      open)"text.txt",11010
            //
            TARGET(OPCODE_OPEN)
            {
                argcGuard(v, 2);
                Str name = Str();
                Tok t = *Vec_at_unsafe(v, 0, Tok);
                // if (t.tokType == Sym){
                //     // borrow
                //     name = t.Sym.sym;
                // }else{
                    long namePtr;
                    try(Tok_getLoc(&t, m, &namePtr));
                    try(Mem_readLtl(m, namePtr, &name));
                // }
                size_t oVal;
                try(Tok_getUint(Vec_at_unsafe(v, 1, Tok), m, &oVal));
                int oflag;
                try(parseOpenOption(oVal, &oflag));
                int fd = open(Str_raw(&name), oflag);
                if (fd == -1){
                    return Error_IoError;
                }
                bool* slot = Vec_at_unsafe(&m->fd, fd, bool);
                if (!slot){
                    return Error_ExceedOpenLimit;
                }
                *slot = true;
                Mem_mem_set(m, 0, &Value(Long, fd));
                DISPATCH()
            }

            TARGET(OPCODE_CLOSE)
            {
                argcGuard(v, 1);
                size_t fd;
                try(Tok_getUint(Vec_at_unsafe(v, 0, Tok), m, &fd));
                bool* slot = Vec_at_unsafe(&m->fd, fd, bool);
                if (!slot || !*slot){
                    return Error_BadFileDescriptor;
                }
                close(fd);
                *slot = false;
                DISPATCH()
            }

            TARGET(OPCODE_SRC)
                DISPATCH()

            TARGET(OPCODE_PRINT_NUM)
            {
                argcGuard(v, 2);
                size_t fd;
                Value value;
                try(Tok_getUint(Vec_at(v, 0, Tok), m, &fd));
                try(Tok_getValue(Vec_at(v, 1, Tok), m, &value));
                if (value.type == 'D'){
                    fprintf(fdopen(fd, "w"), "%f\n", value.Double);
                }else{
                    fprintf(fdopen(fd, "w"), "%ld\n", value.Long);
                }
                DISPATCH()
            }
        }
    }
end:
    return Ok;
}

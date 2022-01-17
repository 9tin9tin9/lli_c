#include "include/opdef.h"
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

Error
exit_(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    long exitCode;
    try(Tok_getInt(Vec_at_unsafe(v, 0, Tok), m, &exitCode));
    exit(exitCode);
}

Error
write_(const Vec* v, Mem* m, Signal* s)
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
    Mem_mem_set(m, 0, Value('L', i));
    *s = Signal(None, 0);
    return Ok;
}

Error
read_(const Vec* v, Mem* m, Signal* s)
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
        Value c = Value('L', buf[i]);
        try(Mem_mem_set(m, desIdx+i, c));
    }
    Mem_mem_set(m, 0, Value('L', i));
    *s = Signal(None, 0);
    return Ok;
}

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

// Open file and set [0] to fd
//      open: name(Ptr | Sym), option(Value)
//
// Open options: number consisting 6 or less digits
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
//  Boolean values will be passed to std::fs::OpenOptions.
//  Read rust docs for more details about each option.
//
//  Example: opening text.txt in read only mode
//      open:"text.txt",1
//
//  Example: opening text.txt in write-only mode, 
//           create file if it does not exists,
//           and will truncate it if it does.
//      open:"text.txt",11010
//
Error
open_(const Vec* v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    Str name = Str();
    Tok t = *Vec_at_unsafe(v, 0, Tok);
    if (t.tokType == Sym){
        // borrow
        name = t.Sym.sym;
    }else{
        long namePtr;
        try(Tok_getLoc(&t, m, &namePtr));
        try(Mem_readLtl(m, namePtr, &name));
    }
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
    Mem_mem_set(m, 0, Value('L', fd));
    *s = Signal(None, 0);
    return Ok;
}

Error
close_(const Vec* v, Mem* m, Signal* s)
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
    *s = Signal(None, 0);
    return Ok;
}

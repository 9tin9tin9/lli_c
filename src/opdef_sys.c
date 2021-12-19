#include "include/opdef.h"
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

Error
exit_(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    long exitCode;
    Error r = Tok_getInt(*Vec_at(v, 0, Tok), *m, &exitCode);
    if (r) return r;
    exit(exitCode);
}

Error
write_(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 3);
    size_t fd;
    Error r = Tok_getUint(*Vec_at(v, 0, Tok), *m, &fd);
    if (r) return r;
    bool* slot = Vec_at(m->fd, fd, bool);
    if (!slot || !*slot){
        return Error_BadFileDescriptor;
    }
    FILE* f = fdopen(fd, "w");
    long srcIdx;
    r = Tok_getLoc(*Vec_at(v, 1, Tok), m, &srcIdx);
    if (r) return r;
    size_t size;
    r = Tok_getUint(*Vec_at(v, 2, Tok), *m, &size);
    if (r) return r;
    size_t i;
    for (i = 0; i < size; i++)
    {
        double value;
        r = Mem_mem_at(*m, srcIdx, &value);
        if (r) return r;
        char ch = value;
        if (!fwrite(&ch, sizeof(char), 1, f))
            return Error_IoError;
        idxIncr(&srcIdx, 1);
    }
    Mem_mem_set(m, 0, i);
    *s = Signal(None, 0);
    return Ok;
}

Error
read_(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 3);
    size_t fd;
    Error r = Tok_getUint(*Vec_at(v, 0, Tok), *m, &fd);
    if (r) return r;
    bool* slot = Vec_at(m->fd, fd, bool);
    if (!slot || !*slot){
        return Error_BadFileDescriptor;
    }
    FILE* f = fdopen(fd, "r");
    long desIdx;
    r = Tok_getLoc(*Vec_at(v, 1, Tok), m, &desIdx);
    if (r) return r;
    size_t size;
    r = Tok_getUint(*Vec_at(v, 2, Tok), *m, &size);
    if (r) return r;
    char buf[MAX_INPUT];
    size_t readSize = fread(buf, sizeof(char), MAX_INPUT, f);
    if (!readSize){
        return Error_IoError;
    }
    size_t i;
    for (i = 0; i < readSize && i < size; i++){
        double c = buf[i];
        r = Mem_mem_set(m, desIdx+i, c);
        if (r) return r;
    }
    r = Mem_mem_set(m, desIdx+i, 0);
    if (r) return r;
    Mem_mem_set(m, 0, i+1);
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

Error
open_(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 2);
    Error r;
    Str name;
    Tok t = *Vec_at(v, 0, Tok);
    if (t.tokType == Sym){
        // borrow
        name = t.Sym.sym;
    }else{
        long namePtr;
        r = Tok_getLoc(t, m, &namePtr);
        if (r) return r;
        r = Mem_readLtl(*m, namePtr, &name);
        if (r) return r;
    }
    size_t oVal;
    r = Tok_getUint(*Vec_at(v, 1, Tok), *m, &oVal);
    if (r) return r;
    int oflag;
    r = parseOpenOption(oVal, &oflag);
    if (r) return r;
    int fd = open(Str_raw(name), oflag);
    if (fd == -1){
        return Error_IoError;
    }
    bool* slot = Vec_at(m->fd, fd, bool);
    if (!slot){
        return Error_ExceedOpenLimit;
    }
    *slot = true;
    Mem_mem_set(m, 0, fd);
    *s = Signal(None, 0);
    return Ok;
}

Error
close_(Vec v, Mem* m, Signal* s)
{
    argcGuard(v, 1);
    size_t fd;
    Error r = Tok_getUint(*Vec_at(v, 0, Tok), *m, &fd);
    if (r) return r;
    bool* slot = Vec_at(m->fd, fd, bool);
    if (!slot || !*slot){
        return Error_BadFileDescriptor;
    }
    close(fd);
    *slot = false;
    *s = Signal(None, 0);
    return Ok;
}

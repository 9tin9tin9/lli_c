#include "include/sys.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

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

#define getArg(n) Vec_at_unsafe(&m->mem, n+2, Value)

Error sys_exit(Mem* m)
{
    long exitCode = getArg(0)->Long;
    exit(exitCode);
}

Error sys_write(Mem* m)
{
    size_t fd = getArg(0)->Long;
    bool* slot = Vec_at_unsafe(&m->fd, fd, bool);
    if (!slot || !*slot){
        return Error_BadFileDescriptor;
    }
    FILE* f = fdopen(fd, "w");
    long srcIdx = getArg(1)->Long;
    size_t size = getArg(2)->Long;
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
    return Ok;
}

Error sys_read(Mem* m)
{
    size_t fd = getArg(0)->Long;
    bool* slot = Vec_at_unsafe(&m->fd, fd, bool);
    if (!slot || !*slot){
        return Error_BadFileDescriptor;
    }
    FILE* f = fdopen(fd, "r");
    long desIdx = getArg(1)->Long;
    size_t size = getArg(2)->Long;
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
    return Ok;
}

Error sys_open(Mem* m)
{
    Str name = Str();
    long namePtr = getArg(0)->Long;
    try(Mem_readLtl(m, namePtr, &name));
    size_t oVal = getArg(1)->Long;
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
    return Ok;
}

Error sys_close(Mem* m)
{
    size_t fd = getArg(0)->Long;
    bool* slot = Vec_at_unsafe(&m->fd, fd, bool);
    if (!slot || !*slot){
        return Error_BadFileDescriptor;
    }
    close(fd);
    *slot = false;
    return Ok;
}

#include "mem.h"

enum SysCallCode {
    SYSCALLCODE_EXIT,
    SYSCALLCODE_READ,
    SYSCALLCODE_WRITE,
    SYSCALLCODE_OPEN,
    SYSCALLCODE_CLOSE
};

Error sys_exit(Mem*);
Error sys_read(Mem* m);
Error sys_write(Mem* m);
Error sys_open(Mem* m);
Error sys_close(Mem* m);

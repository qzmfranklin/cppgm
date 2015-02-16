#pragma once

#include "syscalls.h"

#define PROT_READ       0x1             /* page can be read */
#define PROT_WRITE      0x2             /* page can be written */
#define PROT_EXEC       0x4             /* page can be executed */

#define MAP_SHARED      0x01            /* Share changes */
#define MAP_PRIVATE     0x02            /* Changes are private */
#define MAP_ANONYMOUS   0x20            /* don't use a file */

#define mmap(outaddr, addr, length, prot, flags, fd, offset) syscall6 outaddr SN_MMAP addr length prot flags fd offset

// length must be divisible by 4096 (page size)
#define alloc(outaddr, length) mmap(outaddr, 0, length, /* PROT_READ | PROT_WRITE | PROT_EXEC */ 07, /* MAP_PRIVATE | MAP_ANONYMOUS */ 0x22, (-1), 0)


#pragma once
#include "syscalls.h"

#define stdin 0
#define stdout 1
#define stderr 2

#define read(ret, fd, buf, count) syscall3 ret SN_READ fd buf count
#define write(ret, fd, buf, count) syscall3 ret SN_WRITE fd buf count


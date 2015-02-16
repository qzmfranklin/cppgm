#pragma once

#include "syscalls.h"
#include "alloc.h"
#include "io.h"

#define exit(status) syscall1 t64 SN_EXIT status

 unsigned long preallocate;
 unsigned long _cbrk;
 unsigned long _brk;
 unsigned long _brk_start;

#define ASSERT(exp) 

#include <stdio.h>
#include <sys/mman.h>
#include <limits.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>



// combined sources
#include "ml_freearray.c"
#include "addmem.c"
#include "memmovedown.c"
#include "malloc.c"
#include "free.c"
#include "realloc.c"

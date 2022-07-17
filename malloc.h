// decalarations and macros,


void* malloc_brk(size_t size);
void free_brk(void *m);
void* realloc_brk(void *p, ml_size_t newsize);

#define malloc malloc_brk
#define free free_brk
#define realloc realloc_brk



// definitions 

typedef unsigned int index_t;
// bitsize-1 of maxarea (2GB), and addressable virtual memory
#define BRK_DATATYPE unsigned int
typedef BRK_DATATYPE brk_data_t;


#define BRK_V  0xfffffc
#define BRK_PREVISFREE 0x2
// merged with the index of ml_freearray
#define BRK_FREE 0x1
#define BRK_FREEINDEX 0xfffffffe



// memory blocks to preallocate, to save the syscall overhead of moving the brk
// this value grows progressively for each call to brk
#define BRK_PREALLOCATE 8192
// bitshift preallocate with ((preallocate<<18) & 0x1)
// -> grows 4times, until the value 128MB is reached)
#define BRK_PROGFACTOR 19

// when to free allocated memory by moving the brk.
#define BRK_FREEAT 262144*4


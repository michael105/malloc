// decala
//
#include <stddef.h>




// definitions 

typedef unsigned int index_t;
// bitsize-1 of maxarea (2GB), and addressable virtual memory
#define BRK_DATATYPE unsigned int
typedef BRK_DATATYPE brk_data_t;
typedef BRK_DATATYPE ml_size_t;


// storage of the free elements
struct ml_malloc_freearray{
#ifdef index_X64 
	//( for relative addressing )
		unsigned long base;
#endif
		// overall free memory
		unsigned int memfree;
		unsigned int largestfree;
		unsigned int pos;
		unsigned int data[0]; // starts with index 2
		unsigned int size; // = data[0]
		// count of free elements in the array
		unsigned int freegaps;
		unsigned int _data[];
};


#define SWAP(a,b) a^=b;b^=a;a^=b

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



#define PAGESIZE 4096
#define POINTER unsigned long


#define AC_NORM "\033[0;38;40m"
#define AC_N "\033[0;38;40m"
#define AC_BLACK "\033[0;30m"
#define AC_RED "\033[0;31m"
#define AC_GREEN "\033[0;32m"
#define AC_BROWN "\033[0;33m"
#define AC_BLUE "\033[0;34m"
#define AC_MAGENTA "\033[0;35m"
#define AC_MARINE "\033[0;36m"
#define AC_CYAN "\033[0;36m"
#define AC_LGREY "\033[0;37m"
#define AC_WHITE "\033[0;38m"

void* malloc_brk(ml_size_t size);
void free_brk(void *m);
void* realloc_brk(void *p, ml_size_t newsize);

//#define malloc malloc_brk
//#define free free_brk
//#define realloc realloc_brk




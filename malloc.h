// decala
//
#include <stddef.h>

#define D(v) printf(__FILE__ ": %d   %s " #v " %d\n",__LINE__,__func__,v)
#define Dlx(v) printf(__FILE__ ": %d  %s " #v " %p\n",__LINE__,__func__,v)
#define Dl(v) printf(__FILE__ ": %d  %s " #v " %lu\n",__LINE__,__func__,v)

void* malloc_brk(size_t size);
void free_brk(void *m);
void* realloc_brk(void *p, size_t newsize);

#define malloc malloc_brk
#define free free_brk
#define realloc realloc_brk



// definitions 

typedef unsigned long index_t;
// bitsize-1 of maxarea (2GB), and addressable virtual memory
#define BRK_DATATYPE unsigned long
typedef BRK_DATATYPE brk_data_t;

#define ML_MALLOC_DATATYPE BRK_DATATYPE

// memory blocks to preallocate, to save the syscall overhead of moving the brk
// this value grows progressively for each call to brk
#define BRK_PREALLOCATE 8192
// bitshift preallocate with ((preallocate<<18) & 0x1)
// -> grows 4times, until the value 128MB is reached)
#define BRK_PROGFACTOR 19

// when to free allocated memory by moving the brk.
#define BRK_FREEAT 262144*4


#define BRK_V  0xfffffffffffffffc
#define BRK_PREVISFREE 0x2
// merged with the index of ml_freearray
#define BRK_FREE 0x1
#define BRK_FREEINDEX 0xfffffffffffffffe

#define PAGESIZE 4096
#define POINTER unsigned long

#define SWAP(a,b) a^=b;b^=a;a^=b



// storage of the free elements
struct ml_malloc_freearray{
#ifdef index_X64 
	//( for relative addressing )
		unsigned long base;
#endif
		// overall free memory
		unsigned long memfree;
		unsigned long largestfree;
		unsigned long pos;
		unsigned long data[0]; // starts with index 2
		unsigned long size; // = data[0]
		// count of free elements in the array
		unsigned long freegaps;
		unsigned long _data[];
};


// is located at the start.
struct ml_malloc_area{
	// same as size, when at the top (or bottom)
	// can be negative, when the area grows downwards
	void *pos;
	ML_MALLOC_DATATYPE size;
	ML_MALLOC_DATATYPE free;
	// how much space has been freed, and is located between other allocations
	ML_MALLOC_DATATYPE freegaps;
	// the largest continous area, which has been freed.
	// (eventually already reused)
	ML_MALLOC_DATATYPE largestfree;
};

extern unsigned long  preallocate;
extern unsigned long _cbrk;
extern unsigned long _brk;
extern unsigned long _brk_start;


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




// definitions for malloc_brk and malloc_minibuf

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

//struct ml_malloc_area;
//struct ml_malloc_def;

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

struct ml_malloc_def;

/*
struct ml_malloc_freestorage{	
	void* (*find_free)(struct ml_malloc_def*,int size);
	int (*size)(struct ml_malloc_area*, int index);
	int (*add)(void* addr, int size);
	// the addr of index1 has to be lower than index2 - join right and left
	void (*join)(int index1,int index2);
	void (*attach)(int *addr,unsigned int size, int index);
	void (*clear)(void);
};
*/

// storage of the free elements
struct ml_malloc_freearray{
#ifdef index_X64 
	//( for relative addressing )
		unsigned long base;
#endif
		unsigned int pos;
		unsigned int largestfree;
		unsigned int data[0]; // starts with index 2
		unsigned int size; // = data[0]
		// count of free elements in the array
		unsigned int freegaps;
		unsigned int _data[];
};


struct ml_malloc_def{
	struct ml_malloc_area *area;
	struct ml_malloc_freestorage* free;
 //(at the end of size, is also the global malloc_area registry >> 1)
	// -> minibuf is 0, 	
	// how many bytes need to be free, before to do a real free
	//int freeat;
	// can also change start and current, if mmaps are used
	// returns: 1, when a non continuous area has been allocated.
	//       (-> registerfree of the end)
	// 		0, when the area has been expanded
	
	 // point either to free->find, or malloc_findfree_iterated
	void* (*find_free)(struct ml_malloc_def*,int size);
	int   (*resize_area)(struct ml_malloc_area*,int size);
	int   (*check_area)(struct ml_malloc_area*);
	//int (*emptyarea)(ml_malloc_area *area);
	// 1 or -1 - when bottom and top are reversed
	const int movposfactor;
	const int typesize; //size of datatype in bytes
	// bitmasks 
	const int btval,btfree,btprevfree,bttop;
};

typedef struct ml_malloc_area  ml_malloc_area_t; 


#ifdef __NR_brk
#define BRK
#else
#ifdef SYS_brk
#define BRK
#endif
#endif




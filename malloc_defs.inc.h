// definitions for malloc_brk and malloc_minibuf

//+include
//+def malloc_defs

//+doc type of the minibuf
// alignment and overhead are both sizeof(type)
// unset(default) = unsigned short (16Bit)
// set = unsigned int (32Bit)
//+def MBUF_TYPE_INT 

typedef unsigned int index_t;

#ifdef mini_MBUF_TYPE_INT // int
#define MBUF_DATATYPE unsigned int
#else // shortint
#define MBUF_DATATYPE unsigned short int
#endif

// bitsize-1 of maxarea (2GB), and addressable virtual memory
#define BRK_DATATYPE unsigned int
typedef BRK_DATATYPE brk_data_t;

// memory blocks to preallocate, to save the syscall overhead of moving the brk
// this value grows progressively for each call to brk
#define BRK_PREALLOCATE 8192
// bitshift preallocate with ((preallocate<<18) & 0x1)
// -> grows 4times, until the value 128MB is reached)
#define BRK_PROGFACTOR 19

// when to free allocated memory by moving the brk.
#define BRK_FREEAT 262144*4

#ifdef mini_MALLOC
// full malloc impl.
//#define ML_MALLOC_BITS 32
#define ML_MALLOC_DATATYPE unsigned int
//#define ML_MALLOC_LARGEST_VALUE ((1<<29)-1);
#else
// only minibuf
#define ML_MALLOC_DATATYPE MBUF_DATATYPE
#endif


// memo: (in progress)
//
// ohne malloc, nur minibuf:
// bit 000000fp free, prevfree (32bit) 
// f000p 16 bit -> 32kB max
//
//
// 32bit mit malloc_brk:
// mbuf: bit 1 (right) gesetzt. 
// brk: bit 1 (right) nicht gesetzt
// malloc_brk:  p.....f0 
//  (f = FREE, p=PREVFREE)
// -> 2GB verwaltbar bei 32bit
// freelist in growable mem
// freelist: rel. pointer. 32bit size. -> 8Bytes pro free area
//  -> 64kB bei 8000 free'd areas.
//  -> rep scasq look for free element in array
//
// weitere mallocs > 64MB, oder sowas haben dann halt ein alignment zur pagesize,
// ohne freelist, werden vom kernel verwaltet, 
// sind (theoretisch) auch ueber die adresse identifizierbar.
// -> bits 1....10 (prev+free gesetzt)
//
// bei weiteren splits (evtl sinnvoll) : 
// kein malloc_minibuf.
// malloc_brk ...1
//   ->    f....p1 (ALIGN 4), 1GB max
// malloc_mapped ...0 
//   -> align pagesize. bei 1kB -> 9 bits rechts frei
//   -> bit 9,8: p,f
//   -> bit 7-2 index in array of areas -> max 32 areas
//   -> 4GB max size. 
//   -> (oder: 1GB max -> bits 32,31 pf) -> 128 areas
//     ist wohl besser. wer mehr als 1gb braucht, soll das selbst mappen.
//
//  dann: evtl moeglichkeit fuer reallocs without movement schaffen.
//   -> geht sinnvoll verm nur mit 64 bit header.
//
//   -> malloc, realloc, .. analysieren (macros)
//     -> auswerten, und die macros je nach zeile
//      ersetzen durch jeweiligen malloc call.
//   
//	
//
//
//   array of areas. index by the first(leftmost) bits.
//  
//  32bit: ...pf (align 4bytes)
// -> bis zu vier areas:  512GB
//
// malloc_mmap: 1....110 
//  -> 
// 
// freestorage: rel.(absolute) adressen. -> bits 1,2 (right) -> array of pointers -> absolute
// 	
//
//
// waer noch moeglich, die groesen etc separat zu speichern.
// (4 Byte pro entry), + pointer.
// nur - so kann man die alloziierten areas checken,
// mit 99Prozent wahrscheinlichkeit laest sich sogar die 
// adresse angeben, an der es einen overflow gab.
// -> malloc macros, und es laesst sich auch die zeile im Quellcode
// des jeweiligen malloc angeben im debug modus, nicht nur der segfault.
// (oder: grow.mem, adresse, size und zeilen speichern ->
//  segfault-> lookup addresse - koennen mehrere sein.
//  (bspw after free)


// abstraction of a storage of free areas.
// can be anything, as long the functions below
// are provided.
// I'm going with an unsorted array, located
// at a growable page. Might be fast enough for,
// say, 16k free areas.(=256kB for 64bit). 
// At this notebook.
// "chaostheory"(?) or quite similar at github does have a good looking
// r/b tree implementation. eventually plug that thing here.
// I still do focus on low memory usage. And if you get more than 16k
// free elements, which furthermore are not located at the end of a area,
// youmight be in the need of a key-value store, and better use a "real"
// database.

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

// mbuf definitions =================
#ifdef mini_malloc
// ident bit
#define MALLOC_MBUF 0x1
#ifdef mini_MBUF_TYPE_INT
// 32 bit
#define MBUF_V 0x7FFFFFFc
#define MBUF_FREE 0x80000000
#define MBUF_PREVISFREE 0x2
#define MBUF_DATATYPE unsigned int
#else
// 16 bit
// size mask. 
#define MBUF_V 0x3FFe
#define MBUF_FREE 0x8000
#define MBUF_PREVISFREE 0x4000
#define MBUF_DATATYPE unsigned short int
#endif

#else // only minibuf
#ifdef mini_MBUF_TYPE_INT
#define MBUF_V 0xfffffffc
#define MBUF_FREE 0x2
#else
#define MBUF_V 0x7FFe
#define MBUF_FREE 0x8000
#endif
#define MBUF_PREVISFREE 0x1
#endif // only minibuf
//===================================

#define MBUF_TOP 0
//#define MBUF_TOP MBUF_FREE
#define MBUF_MAX MBUF_V


#if 0
#define BRK_V  0x3FFFFFFe
#define BRK_PREVISFREE 0x40000000
// merged with the index of ml_freearray
#define BRK_FREE 0x80000000
#define BRK_FREEINDEX 0x3ffffffe
#else
#define BRK_V  0xfffffc
#define BRK_PREVISFREE 0x2
// merged with the index of ml_freearray
#define BRK_FREE 0x1
#define BRK_FREEINDEX 0xfffffffe
#endif



#define MALLOC_FREE 0x80000000
#define MGR_FREE MBUF_FREE

//#define MBUF_OCC 0x40000000
//#define MGR_OCC MBUF_OCC
//#define MALLOC_OCC MBUF_OCC

// at the end of the brk
//#define MALLOC_BRK_END MBUF_OCC
//#define BRK_END MBUF_OCC

// simple checkvalue whether a area is free or occupied.
// If neither nor, most possibly there's a problem.
#define MBUF_CHK 0xC0000000   

#define MALLOC_PREVISFREE 0x2

#define MGR_PREVISFREE MALLOC_PREVISFREE

// has been allocated with brk
#define MALLOC_BRK 0x0

#define MGR_V MBUF_V 
#define MALLOC_SIZE MBUF_V





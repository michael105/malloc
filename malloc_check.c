//+doc check integrity of malloc, and pointers.
// basically iterate over all areas, which have been
// allocated with malloc.
// if there has been a overrun, the saved sizes
// would be changed. 
// By default, this function will return 1 in this 
// case, and dump out the last memory pointer,
// which seems to be ok.
// In favor of performance, there's no separate
// memory area for storing the sizes.
// Therefore the performance of malloc_grow and
// free might be close to variables at the stack.
//
// if debug = 1, the switch DEBUG is set, dump informative messages
//  (printf, fmtp and fmtd need to be enabled as well)
// p can be 0, the whole allocated memory is looped over,
// or any allocated pointer.
//
// returns 0, if everything seems to be ok,
// or 1, if not.
//
// TODO: define global DEBUG switch.
//
//+depends ewrites
//+def
int malloc_check(ulong* start, int debug){
#ifdef DEBUG
#define _mgrdbg(...) if (debug) dbgf(__VA_ARGS__)
#else
#define _mgrdbg(...) 
#endif

	char *p = (char*)(mlgl->mbufarea->pos);
	char *end = mlgl->mbufarea->pos + ( mlgl->mbufarea->size - mlgl->mbufarea->free);// - sizeof(MBUF_DATATYPE) );//(char*)mlgl->mbufarea - sizeof(MBUF_DATATYPE);
	
	_mgrdbg(AC_LGREEN"\nMalloc - allocated areas\n");
	_mgrdbg(AC_LGREEN"area: size: %d free: %d  freegaps: %d  largestfree: %d\n",
		mlgl->mbufarea->size,mlgl->mbufarea->free,mlgl->mbufarea->freegaps,mlgl->mbufarea->largestfree);
	_mgrdbg(AC_NORM AC_BOLD"bottom: %p  top: %p  area used: %d\n",p,end,(end-p));
	
	unsigned int freed = 0;
	unsigned int allocated = 0;


	while ( p<(char*)end ){
		_mgrdbg(AC_YELLOW"  addr: %p  size: % 3d",p, (*(MBUF_DATATYPE*)p & MBUF_V) );
		if ( *(MBUF_DATATYPE*)p & MGR_FREE ){
			_mgrdbg(AC_LCYAN AC_BOLD" (free)" );
			freed+=(*(MBUF_DATATYPE*)p & MGR_V);
		} else {
			allocated+=(*(MBUF_DATATYPE*)p&MGR_V);
		}
		if ( *(MBUF_DATATYPE*)p & MBUF_PREVISFREE )
			_mgrdbg(AC_LGREEN" (previsfree)" );
		_mgrdbg("\n");

		MBUF_DATATYPE *op = (MBUF_DATATYPE*)p;
		p= p+(*(MBUF_DATATYPE*)p & MBUF_V);
		if ( (MBUF_DATATYPE*)p == op ){
			ewrites(AC_LRED"malloc_check: error: size = 0 \n"AC_NORM);
			return(1);
		}
	}
	_mgrdbg(AC_WHITE AC_BOLD"top:    %p  = %x\n",end,*(MBUF_DATATYPE*)end);
	_mgrdbg(AC_LGREEN"Allocated: %d  Free areas: %d\n",allocated,freed);

	if ( p != (char*)end ){
		ewrites(AC_LRED"malloc_check: Error\n\n"AC_NORM);
		return(1);
	}
	_mgrdbg(AC_LGREEN"check: Ok\n\n"AC_NORM);
	return(0);
}

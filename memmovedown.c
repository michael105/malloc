// copy overlapping areas, from upper to lower.
// (s>d)
// This does vector copy operations of longs, 64bit with amd64
MF void memmovedown(void *d, void *s, ml_size_t size){
#define ASSERT(exp) 
	ASSERT(d<ULONG_MAX-sizeof(long));
	ASSERT(d<s);

	if ( d>=s )
		ABORT;

	void *e = d+size;

	while ( d < e-sizeof(long) )
		*(long*)d++ = *(long*)s++;

	while ( d<e )
		*(char*)d++ = *(char*)s++;
}


// copy overlapping areas, from upper to lower.
// (s>d)
// This does vector copy operations of longs, 64bit with amd64
void memmovedown(void *d, void *s, size_t size){
	ASSERT(d<ULONG_MAX-sizeof(long));
	ASSERT(d<s);

	if ( d>=s )
		exit(1);

	void *e = d+size;

	while ( d < e-sizeof(long) )
		*(long*)d++ = *(long*)s++;

	while ( d<e )
		*(char*)d++ = *(char*)s++;
}


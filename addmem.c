
int ml_brk(void* p){
			printf("m; brk: %p\n",_brk);
			printf("m; p: %p\n",p);
			printf("size: %ld\n",((ulong)p -(ulong)_brk)/1024/1024);
	if ( brk(p)  == 0 ){ 
		_brk = p;
		return(0);
	}
	return(-1);
}
//+doc move cbrk (malloc_brk internal)
//+def 
int ml_addmem(void* brkmin){
		// calculate memory to preallocate
		preallocate <<=1;
		// grow progressively
		preallocate += ((preallocate>>BRK_PROGFACTOR)<<BRK_PROGFACTOR);
		// max is 1<<27 == 132MB
		preallocate = ((preallocate-1) & ((1<<27)-1) )+1;
			printf("pre: %d\n",preallocate);
		// round up to the next page 
		if ( ml_brk( (void*)(((ulong)brkmin + (preallocate +PAGESIZE-1) ) & (~(PAGESIZE-1))) ) != 0 ){
			printf("p: %p\n",_brk);
			printf("errno: %d\n",errno);
			printf("brk: %p\n",sbrk(0));
			warn("NOMEM2\n");
			return(0);
		}
		return(1);
}




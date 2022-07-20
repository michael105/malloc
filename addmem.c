//+doc move cbrk (malloc_brk internal)
//+def 
int ml_addmem(void* brkmin){
		// calculate memory to preallocate
		preallocate <<=1;
		// grow progressively
		preallocate += ((preallocate>>BRK_PROGFACTOR)<<BRK_PROGFACTOR);
		// max is 1<<27 == 132MB
		preallocate = ((preallocate-1) & ((1<<27)-1) )+1;
		// round up to the next page 
		if ( brk( (void*)((POINTER)(brkmin + (preallocate +PAGESIZE-1) ) & (~(PAGESIZE-1))) ) < 0 ){
			warn("NOMEM2\n");
			return(0);
		}
		return(1);
}


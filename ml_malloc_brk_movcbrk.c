//+doc move cbrk (malloc_brk internal)
//+def 
MF int ml_malloc_brk_addmem(void* brkmin){
		// calculate memory to preallocate
		mlgl->brk_preallocate <<=1;
		// grow progressively
		mlgl->brk_preallocate += ((mlgl->brk_preallocate>>BRK_PROGFACTOR)<<BRK_PROGFACTOR);
		// max is 1<<27 == 132MB
		mlgl->brk_preallocate = ((mlgl->brk_preallocate-1) & ((1<<27)-1) )+1;
		//printf("preallocate: %d\n",mlgl->brk_preallocate);
		printf("preallocate: %d, address min: %p\n",mlgl->brk_preallocate,brkmin);
		// round up to the next page 
		if ( brk( (void*)((POINTER)(brkmin + (mlgl->brk_preallocate +PAGESIZE-1) ) & (~(PAGESIZE-1))) ) < 0 ){
			warn("NOMEM2\n");
			return(0);
		}
		return(1);
}


#define CAST(var) (BRK_DATATYPE*)(var)
#define BRKSZ sizeof(BRK_DATATYPE)


//+doc allocate via setting the brk
// free and realloc can be used normally.
// The intention of malloc_brk is for subsequent calls to realloc.
// The saved data has not to be copied,
// instead realloc just writes the new size and sets 
// the brk accordingly.
// if the break is saved before one or more calls to malloc_brk,
// the allocated memory can also be free'd by setting the brk to the saved value
// with brk(saved_brk)
// free_brk() free's all memory, which has been allocated with malloc_brk
//+depends brk getbrk malloc_defs warn ml_freearray ml_malloc_brk_addmem
//+def
MF void* malloc_brk(ml_size_t size){
	void *addr;
	brk_data_t esize;
	index_t reuse; 

	// alignment and reserving space for the relative pointer/size and array index
	size = ((size-1)& ~(BRKSZ-1))+(2*BRKSZ );

	// look for a fitting area
	if (( reuse = ml_find(size,&addr,&esize) )){
		if ( (esize - size) <= (2*BRKSZ) ){ // too less left
			ml_remove(reuse);
			size = esize; // avoid fragmentation
			*CAST(addr+esize) &= (~BRK_PREVISFREE); // clear prevfree
		} else {
			brk_data_t *newaddr = ml_shrink(size,reuse); // move free area right
			*newaddr = reuse | BRK_FREE;
			// index prevfree is already stored
		}
	} else { // append new area
		if ( ( mlgl->cbrk + size + BRKSZ >= mlgl->brk ) &&
				!ml_malloc_brk_addmem((void*)(mlgl->cbrk+size+BRKSZ)) ) 
			return(0);

		addr = (void*)(mlgl->cbrk);
		mlgl->cbrk += size;
		*CAST(mlgl->cbrk) = 0;
	}

	*CAST(addr) = size;
	return( addr + BRKSZ );
}


#undef CAST


void* realloc_brk(void *p, size_t newsize){
#define CAST(var) (BRK_DATATYPE*)(var)
#define CUR CAST(m)
#define BRKSZ sizeof(BRK_DATATYPE)

	if ( !newsize ){
		free_brk(p);
		return((void*)0);
	}
	//
	// alignment and reserving space for the relative pointer/size and array index
	newsize = ((newsize-1)& ~(BRKSZ-1))+(2*BRKSZ );

	if ( p == 0 )
		return( malloc_brk(newsize) );

	void *m = p - BRKSZ;
	size_t oldsize = *CUR & BRK_V;

	if ( *CUR & BRK_FREE ){
		warn("double free or overflow");
		exit(1);
	}

	// same size, or too less left to be reused
	if ( (newsize == oldsize) || ( abs(newsize-oldsize) <= (2*BRKSZ) ) )
		return(p);

	// shrink, add or join new free area
	if ( newsize<oldsize ){
		index_t e;
		if ( *CAST(m+oldsize) & BRK_FREE ){
			// next is free
			e = *CAST(m+oldsize) & BRK_FREEINDEX;
			ml_expand( oldsize-newsize, e );
		} else {
			 // new free area
			 e = ml_add(m+newsize,oldsize-newsize);
			 // set prevfree
			*CAST(m+oldsize-BRKSZ) = e;
			*CAST(m+oldsize) |= BRK_PREVISFREE;
		}
		*CAST(m+newsize) = e | BRK_FREE;
		*CUR = newsize;
		return(p);
	}


	// enlarge. at the currrent break. move break and return
	if ( *CAST( m + oldsize ) == (BRK_DATATYPE)(POINTER)_cbrk ){
		if ( m + newsize >= (void*)_brk ){
			if ( !ml_addmem(m+newsize+BRKSZ) ){
				return(0);
			}
		}

		*CUR = newsize;
		_cbrk = (typeof(_cbrk))(m + newsize);
		*CAST(_cbrk) = 0;

		return(p);
	}

	void *addr;
	size_t prevsize;
	if ( (*CUR & BRK_PREVISFREE) && 
			( (prevsize=ml_get_size(*CAST(m-BRKSZ))) >= newsize - oldsize - BRKSZ ) ){
		// prev is free and large enough.
		// would be possible to have two large areas, but a third wouldn't fit into the memory
		addr = ml_shrink_down( newsize-oldsize, *(CAST(m-BRKSZ)) );
		// areas (possibly) overlap, and eventually don't fit a third (or second) time into the memory
		// neither memcpy nor memmove are usable here
		memmovedown((addr+BRKSZ),p,(oldsize-BRKSZ));
		
		*CAST(addr) = newsize;
		return(addr+BRKSZ);
	}

	addr = malloc_brk(newsize);
	if ( !addr ){
		warn("realloc: NOMEM");
		return((void*)0);
	}

	memcpy( addr, p, oldsize-BRKSZ );
	free_brk( p );
	return(addr);
}

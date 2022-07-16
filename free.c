
#define xxprintf {}
#define CAST(var) (BRK_DATATYPE*)(var)
#define CUR CAST(m)
#define BRKSZ sizeof(BRK_DATATYPE)

//+doc 
//+def
MF void free_brk(void *m){
	if ( !m ) 
		return;

	// point to the size of the area
	m -= BRKSZ;

	if ( (*CUR & BRK_FREE) || (*CUR == 0) ){
		warn("Double free or Overflow\n");
		return;
	}

	// store size (evtl asm bts)
	brk_data_t size = *CUR & BRK_V;

	// at the end of the current break
	if ( m+size == (void*)mlgl->cbrk  ){
		// remove prev, if free
		if ( *CUR & BRK_PREVISFREE ){
			index_t e = *(CUR-1);
			m = ml_get_addr(e);
			ml_remove(e);
		}

		mlgl->cbrk = (typeof(mlgl->cbrk))m;
		*CUR = 0;

		// release memory
		if ( mlgl->brk - mlgl->cbrk > (mlgl->brk_preallocate>>2) ){
			mlgl->brk_preallocate >>= 2;

			// lowest possible value is BRK_PREALLOCATE
			mlgl->brk_preallocate = ((mlgl->brk_preallocate-1) + BRK_PREALLOCATE) & ~(BRK_PREALLOCATE-1);

			// free
			brk( (void*)((mlgl->cbrk + (BRK_PREALLOCATE +PAGESIZE-1) ) & (~(PAGESIZE-1))) );
		}
		return;
	}

	// add to prev, if free
	if ( *CUR & BRK_PREVISFREE ){

		index_t e = *(CUR-1);
		*CUR |= BRK_FREE; // set to check for double frees later

		m = ml_attach(&size,e);

		// next also free
		if ( *CAST(m + size ) & BRK_FREE ){
			e = ml_join(e,(*CAST(m + size ) & BRK_FREEINDEX ),&size ); 
			*CUR = e | BRK_FREE;
		} else {
			*CAST(m + size ) |= BRK_PREVISFREE;
		}
		// write prevfree
		*CAST(m + size - BRKSZ ) = e;
		return;
	}

	// next is free
	if ( *CAST(m + size ) & BRK_FREE ){
		ml_expand(size,((*CAST(m + size )) & BRK_FREEINDEX ) ); 
		*CUR = *CAST(m + size );  // write index 
		//*(BRK_DATATYPE*)((void*)p + size ) = 0; // delete (set free for check)
		return;
	}

	index_t e = ml_add(m,*CUR);
	// store index
	*CAST(m+*CUR - BRKSZ) = e;
	*CAST(m+*CUR ) |= BRK_PREVISFREE;
	*CUR = e | BRK_FREE;

	return;
}
#undef CUR
#undef CAST
#undef BRKSZ
#undef xxprintf

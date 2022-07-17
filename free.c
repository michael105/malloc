#define CAST(var) (BRK_DATATYPE*)(var)
#define CUR CAST(m)
#define BRKSZ sizeof(BRK_DATATYPE)

void free_brk(void *m){
	if ( !m ) 
		return;

	// point to the size of the area
	m -= BRKSZ;

	printf(" BRK: %lx\n",brk);
	printf("CBRK: %lx\n",_cbrk);
	printf("CUR:  %lx\n",(ulong*)m);

	if ( (long)_cbrk - (long)m < 0 ){ printf(AC_RED"!!!!!!!!!!!!!");}
	printf(AC_CYAN "CUR: %lx\n",*(long*)m);
	if ( (*CUR & BRK_FREE) || (*CUR == 0) ){
		warn("Double free or Overflow\n");
		return;
	}

	// store size (evtl asm bts)
	brk_data_t size = *CUR & BRK_V;
	printf("size %ld\n",size);

	// at the end of the current break
	if ( m+size == (void*)_cbrk  ){
		printf("===\n");
		// remove prev, if free
		if ( *CUR & BRK_PREVISFREE ){
			index_t e = *(index_t*)(m-8);
		D(e);
			m = ml_get_addr(e);
			ml_remove(e);
		}

		_cbrk = (typeof(_cbrk))m;
		*CUR = 0;

		Dlx(_cbrk);
		Dlx(m);
		// release memory
		if ( _brk - _cbrk > (preallocate>>2) ){
			preallocate >>= 2;


			// lowest possible value is BRK_PREALLOCATE
			preallocate = ((preallocate-1) + BRK_PREALLOCATE) & ~(BRK_PREALLOCATE-1);
			printf(AC_GREEN"==================\n"AC_N);
Dlx(_brk);
			// free
			ml_brk( (void*)(((POINTER)_cbrk + (BRK_PREALLOCATE +PAGESIZE-1) ) & (~(PAGESIZE-1))) );
			_brk = sbrk(0);
Dlx(_brk);
		}
		return;
	}

	// add to prev, if free
	if ( *CUR & BRK_PREVISFREE ){

		index_t e = *(CUR-1);
		*CUR |= BRK_FREE; // set to check for double frees later

		D(size);
		printf("m: %p\n",m);
		m = ml_attach(&size,e);
		printf("m: %p\n",m);
		D(size);

		// next also free
		if ( *CAST(m + size ) & BRK_FREE ){
			e = ml_join(e,(*CAST(m + size ) & BRK_FREEINDEX ),&size ); 
			*CUR = e | BRK_FREE;
		} else {
			*CAST(m + size ) |= BRK_PREVISFREE;
		}
		if ( !e )
			exit(7);
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
		if ( !e )
			exit(8);

	// store index
	*CAST(m+*CUR - BRKSZ) = e;
	ulong sz = *CUR;
	Dl(sz);
	*CAST(m+*CUR ) |= BRK_PREVISFREE;
	*CUR = e | BRK_FREE;

	return;
}
#undef CUR
#undef CAST
#undef BRKSZ

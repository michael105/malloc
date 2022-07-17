// functions, to manage a dynamically growing array of free areas




// a dynamic array, located in a growing mapping. (mmap - MAP_GROWSDOWN)
// index of the first element starts with 2,
// and counts in steps of 2. -> there is a (2) bit free
// to indicate whether a allocated area is used, and contains the len
// of the area (aligned to 4), or if the area is free, and the number
// contains the index in this array. (Bit 2 set).
//
// Bit 1 currently is the indicator for an area at the minibuf(set),
// or within malloc_brk (unset).
//
// data[2] contains the size, data[3] the address, ..
// The content of empty elements is set to 0.
// the array compacts itself by adding new elements into gaps, if present,
// and joining elements "downwards".
//
// Same time, another (sort of)"half double linked"list of free areas is written into
// free'd memeory areas, containing the indexes of this array.
// So it is possible to iterate forward through allocated and free'd areas,
// and go backwards for one free element within mallocated areas,
// to join adjacent free areas.
//
// (Would be obviously possible to use a real double linked list,
// but the overhead seems to me unneccessary. 
// The are two cases,
// when it is important to know, how large the previous area is.
// If the area had already been freed and needs to be joined with the next freed area,
// and if a reallocation fits into the old and the joined neighbouring area,
// the locality is advantegeous.
//

static struct ml_malloc_freearray *__freearray; 

#define PA __freearray

//+def
struct ml_malloc_freearray* ml_freearray_new(){
	// grows upwards  as well, despite the flag's naming
	__freearray = (struct ml_malloc_freearray*) mmap(0,PAGESIZE,PROT_READ|PROT_WRITE,
			MAP_ANONYMOUS|MAP_PRIVATE|MAP_GROWSDOWN,-1,0);
	return(__freearray);
}

//+def
size_t ml_remove(index_t index){
	size_t size = PA->data[index];

	PA->memfree -= PA->data[index];
	*(uint64_t*)(PA->data+index) = 0;
	//PA->data[index] = 0;
	if ( index == PA->pos ){ // was at the end
		PA->pos -= 2;
		// shrink array down to first used element
		while ( (PA->data[PA->pos] == 0) && PA->freegaps ){
			PA->freegaps--;
			PA->pos -= 2;
		}
	} else {
		// new empty element
		PA->freegaps++;
	}
	return(size);
}


// join two elements, which are adjacent in memory
//+def
index_t ml_join(index_t lower, index_t upper, brk_data_t *newsize){
	if ( lower > upper ){ // order of the areas in memory and within this array are swapped
		// compact the array by adding the higher index to the lower index
		SWAP(lower,upper);
		// move addr
		PA->data[lower+1] = PA->data[upper+1];
	}

	PA->data[lower] += PA->data[upper];
	*newsize = PA->data[lower];
	PA->memfree += PA->data[upper];
	ml_remove(upper);
	return(lower);
}

// attach area to a free area below
// return the address
//+def
void* ml_attach(brk_data_t *size, index_t tolower){
	D(tolower);
	D(*size);
	PA->memfree += *size;
	PA->data[tolower] += *size;
	*size = PA->data[tolower];
	return((void*)(PA->data[tolower+1]));
}

// attach area to free area above. move the address of the upper free area.
//+def
void ml_expand(brk_data_t diff, index_t uppertomove){
	PA->memfree += diff;
	PA->data[uppertomove]+=diff;
	PA->data[uppertomove+1]-=diff; // move addr
}

// shrink (and move upwards)
//+def
brk_data_t* ml_shrink(brk_data_t diff, index_t e){
	PA->memfree -= diff;
	PA->data[e] -= diff; // save new size
	PA->data[e+1] += diff; // move addr
	return( (brk_data_t*)(ulong)PA->data[e+1] );
}


// shrink (and "move" downwards in memory, shrink the size)
// return the new area start (right of)
//+def
brk_data_t* ml_shrink_down(brk_data_t diff, index_t e){
	PA->memfree -= diff;
	PA->data[e] -= diff; // save new size
	//PA->data[e] += diff; // move addr
	return( (brk_data_t*)(POINTER)(PA->data[e+1]+PA->data[e]) );
}


// return the size of an area. (Usable is size - BRKSZ)
//+def
size_t ml_get_size(index_t e){
	return(PA->data[e]);
}

// get a pointer to an area. (usable memory starts at p+BRKSZ)
//+def
brk_data_t* ml_get_addr(index_t e){
	return((brk_data_t*)(POINTER)(PA->data[e+1]));
}


// insert or append a new element
//+def
index_t ml_add(void *addr, brk_data_t size){
	D(size);
	printf("addr: %p\n",(long)addr);
	D(size);
#ifdef X64
	if ( (uint32_t)((uint64_t)addr) != (uint64_t)addr ){ // bit in the upper 32bit is set
		// more than 32bit
		ERR("NOMEM5","Error - addr too high\n",EFAULT);
	}
#endif
	if ( size > PA->largestfree )
		PA->largestfree = size;

	index_t index = PA->pos+2;
	if ( PA->freegaps > 0 ){ // > threshhold
		// find empty element
		brk_data_t* data = PA->data + 2;
		index >>= 1;
		asm volatile("repne scasq" : "+D"(data), "+c"(index) : "a"(0) : "cc");
		index = (data - PA->_data); // index 

		if ( index==0 ) 
			exit(9);
		if(index>=PA->pos){
			write(2,"index too high\n",16);
			exit(EFAULT);
	}
		PA->freegaps--;
	} else {
		PA->pos+=2;
		D(index);
	}

	PA->memfree += size;
	PA->data[index] = size;
	D(index);
	PA->data[index+1] =  (brk_data_t)((POINTER)addr); 
	// eventually shift right -> 16GB addressable ("34bit")
	// right shift 3 : (align 8) 32GB
	return(index);
}


// find best fitting free area, which is larger than size.
//+def
index_t ml_find(brk_data_t size,void **addr,brk_data_t *esize ){
	if ( (size > PA->largestfree) || (size > PA->memfree) ){
		return(0);
	}
	index_t _max = ULONG_MAX,*bestfit = &_max,largest=0;

	for ( index_t *i = PA->_data, *e = (PA->data+PA->pos); i<=e; i+=2 ){
		//xxprintf("sz: %d *i: %d %u\n",size,*i,*bestfit);
		if ( *i == size ){
			bestfit = i;
			break;
		}
		if ( *i > largest )
			largest = *i;
		if ( (*i > size) && (*i <= *bestfit) ) // go right, if equal -> free elements "move" left
			bestfit = i;
	}
	PA->largestfree = largest; // update largestfree
	// has not neccessarily to be free, but there is no larger free area

	if ( bestfit != &_max ){ //found
		*esize = *bestfit;
		*addr = (void*)(POINTER)*(bestfit + 1);
		return ( bestfit - PA->data );
	}
	return(0);
}



#undef xxprintf	






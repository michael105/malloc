#include "malloc.h"
#include "sources.c"

#define REP 32
#define REPB 100
#define ASIZE 256
#define REALLOC 32
#define MEMSIZE 0xff


int main(int argc, char **argv){
	ml_freearray_new();
	_brk = sbrk(0);
	preallocate = BRK_PREALLOCATE;
	_brk_start=_cbrk=_brk;

	printf("%p = %p\n",_cbrk,_brk);
	
	srand(12345);

	int *m[ASIZE];
	int set[ASIZE];


	for ( int a = 0; a<REP; a++ ){
		for ( int b = 0; b< ASIZE; b++ ){
			m[b] = malloc_brk((rand()&MEMSIZE)+1);
			printf("mall addr, %d: %p\n",b,m[b]);
			char *c = m[b];
			long *i = (long*)(c-8);
			//printf("sz:  = %ld\n",*i);
			printf("sz i:  = %p\n",i);
		}
		memset(set,1,sizeof(memset));

		for ( int b = 0; b<REPB; b++ ){
			int t1 = rand()&(ASIZE-1);
			int t2 = rand()&MEMSIZE;
			if ( set[t1] )
				free_brk(m[t1]);
			else 
				m[t1] = malloc_brk(t2);
			set[t1] = !set[t1];
		}


		printf(AC_RED"===============\n\n"AC_N);
		for ( int b = 0; b< ASIZE; b++ ){
			printf(AC_BLUE"free: %d\n"AC_NORM,b);
			printf("m = %p\n",m[b]);
			if ( set[b] )
				free_brk(m[b]);
		}

	}

	


	exit(0);
}

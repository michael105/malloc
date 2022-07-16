Release erlay, release often.

Once a wise man said.


I guess, there's a difference between open source and music.


anyways, here is a early release of "malloc".



Yet this showed up with minimal overhead. 
4 Bytes per allocation, and 8 Bytes for the free list(array).

Only 32bit yet. 64bit to come.

This malloc implemention uses explicitely the brk for allocations,
besides one dynamically growing map for the freelist.


Yet I'm not that proud about the abstraction - its missing.
To be more exact, I'd like to 1. have simple map calls for larger allocations,
and some analysis about the usage and according placement also into maps would be great.

Yet, there is a progressively growing brk implemented, to spare unneccessary syscalls.


This malloc is fast. The free array shows up to be neglecticle in overhead.

I do guess, beginning with ? 10.000 free elements, you should better look for another way
of the memory management. (also using rep scasq assmbly instructions, sort of "optimized")


I compared this to the kkmalloc implementation, in the question of speed there's no measurable
difference. However, this implementation uses 1/3 less memory, for a bunch of randomized allocations and free's.


The concept is to have a usual malloc area.
As soon, elements are freed, they are noted in the adjacent elements,
and stored in the freearray.

Looking for the possiblity of reallocations or joining free elements
within the memory there is the next element, and the previous, if free, reachable.
(within the memory)
Sort of a halfway linked list.

Looking for a free area gets to a scan through the array of free elements,
depending on the usecase, this is a scan through maybe one Page. (4096 Byte,500 free elements for 32bit)

The array of free elements compacts itself with randomized usage.
Some unnecessary grow I could determine only with several thousand random reallocations.


Albite in this case, implementing the "realloc" as malloc - memcpy - free -
solved this "problem".

The current malloc reuses adjacent free areas for realloc, ideally it even doesn't need to copy memory or change the address.
However, also obviously, this comes with some fragmentation.

Albite this only matches "unusual" usecases, several 10 to 100 thousand randomized allocations, free's, reallocs.

somehow I'm fed up with my corona mood - sitting there and hacking,
since there's nothing out there.

This malloc therefore is "finished" - in the meaning of "working".
luckily the 64bit implementation is only a change to some macros.

Hasn't been so much of important yet, since this malloc is originally a part of my minilib project.
(Therefore I concentrated on the overhead, with the repetitive result, strictly(mainly) optimizing for memory
also shows up to be quite performant as a side effect)..











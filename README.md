


Currently this works only for address below 32bit,
uclibc and glibc do have adresses above.
(This is a split of minilib, with minilib the brk is below 32bit)


It would be possible to use relative addresses,
but I cannot see enough of an advantage for the usage
with glibc or the other libs. The spared memory
might not outwage penalty of misaligned access, etc.

So I leave this at it is, going to fix the 64bit version.







I do guess, beginning with ? 10.000 free elements, you should better look for another way
of the memory management. (also using rep scasq assmbly instructions, sort of "optimized")


I compared this to the kmalloc implementation, in the question of speed there's no measurable
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











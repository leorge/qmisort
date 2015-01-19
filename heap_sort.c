/*
 * heap_sort.c
 *
 *	Heap sort
 *
 *  Created on: 2013/06/06
 *      Author: leo
 */

#include "sort.h"

static	size_t	length;
static int 	(*comp)(const void *, const void *);

/* wrapper of memcpy() */
static void copy(void *dst, const void *src)
{
#ifdef DEBUG
	qsort_moved++;
	if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %p, src = %p : %s)\n", dst, src, dump_data(src));
#endif
	memcpy(dst, src, length); /* restore an elements  */
}

static char	*first, *last, *key;
size_t	anchor;

static void heap(size_t nmemb, size_t node)
{
#ifdef DEBUG
	dump_array("heap start", first, nmemb, length);
#endif
	char	*child, *start = first + node * length, *parent = start;
	size_t	leaf;
#ifdef DEBUG
		qsort_called++;
#endif
	while(TRUE) {
		leaf = (node << 1) + 1;			// 2n + 1
		if (leaf >= nmemb) break;		// no child
		child = first + leaf * length;
		if (leaf + 1 < nmemb && comp(child + length, child) > 0 ) {
			child += length;
			leaf++;
		}
		if (comp(key, child) >= 0) break;
#ifdef DEBUG
		qsort_called++;	// recursive call
#endif
		copy(parent, child); parent = child;	//slide
		node = leaf;
	}
	if (parent > start) copy(parent, key);		// restore
#ifdef DEBUG
	dump_array("heap done", first, nmemb, length);
#endif
}

void heap_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
	if (nmemb <= 1) return;
#ifdef DEBUG
	size_t	initial_size = nmemb;
	qsort_called++;
#endif
	comp = compar;
	char	a[length = size]; key = a;
	first = (char *)base;
	last = first + (nmemb - 1) * size;	// next element of the last element
	size_t node = (nmemb - 1) >> 1;		// lowest parent node
	do {	// build up a heap
		copy(key, first + node * size);
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "key = %s\n", dump_data(key));
#endif
		heap(nmemb, node);
	} while (node--);
	do {	// sort
		copy(key, last); copy(last, first); copy(first, key);	// *key <-- *last <-- *first <-- *key
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "key = %s\n", dump_data(key));
#endif
		last -= size; heap(--nmemb, 0);		// shrink and re-build the heap
	} while (nmemb);
#ifdef DEBUG
	dump_array("heap_sort done", first, initial_size, size);
#endif
}

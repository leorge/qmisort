/*
 * heap_sort.c
 *
 *  Heap sort
 *
 *  Created on: 2013/06/06
 *      Author: leo
 */

#include "sort.h"

static int  (*comp)(const void *, const void *);

static void **root, *key;
//size_t  anchor;

static void heap(size_t nmemb, size_t node)
{
#ifdef DEBUG
    dump_pointer("heap start", root, nmemb);
    qsort_called++;
#endif
    void    **child, **start = root + node, **parent = start;
    size_t  leaf;
    while(TRUE) {
        leaf = (node << 1) + 1;         // 2n + 1
        if (leaf >= nmemb) break;       // no child
        child = root + leaf;
        if (leaf + 1 < nmemb && comp(*child, *(child + 1)) < 0 ) {
            child++;
            leaf++;
        }	// right leaf is larger than left leaf
        if (comp(key, *child) >= 0) break;
        *parent = *child; parent = child;    //slide
        node = leaf;
    }
    if (parent > start) *parent = key;      // restore
#ifdef DEBUG
    dump_pointer("heap done", root, nmemb);
#endif
}

// sort
static void sort(size_t nmemb) {
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("sort()", root, nmemb);
#endif
	size_t  n = nmemb;
	void **last = root + n - 1;  	// next element of the last element
    do {    // sort
    	key = *last; *last = *root; *root = key;	// swap first <--> last
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "key = %s\n", dump_data(key));
#endif
        last--; heap(--n, 0);     // shrink and re-build the heap
    } while (n);
}

// bulld a heap by shift up
void heap_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("sort() start in " __FILE__, base, nmemb);
#endif
    for (size_t node = 1; node < nmemb; node++) {
#ifdef DEBUG
        qsort_called++;
#endif
    	size_t	child = node;
        size_t	parent = (child - 1) >> 1;
        while (compare(base[parent], base[child]) < 0) {
#ifdef DEBUG
        	if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap : %s [%ld] <--> %s [%ld]\n"
        			, dump_data(base[parent]), parent, dump_data(base[child]), child);
#endif
        	void *tmp = base[parent]; base[parent] = base[child]; base[child] = tmp;
        	if (! parent) break;	// root
        	child = parent; parent = (child - 1) >> 1;
        }
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "A heap is built. compare = %ld\n", qsort_comp_str);
#endif
	comp = compare;
	root = base;
	sort(nmemb);
#ifdef DEBUG
    dump_pointer("heap_sort done", root, nmemb);
#endif
}

// build a heap by shift down
void heap_sort2(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("heap_sort2() start in " __FILE__, base, nmemb);
#endif
    comp = compare;
    root = base;
    size_t node = (nmemb - 1) >> 1;  	// lowest parent node
    do {
        key = *(root + node);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "key = %s\n", dump_data(key));
#endif
        heap(nmemb, node);
    } while (node--);
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "A heap is built. compare = %ld\n", qsort_comp_str);
#endif
    sort(nmemb);
#ifdef DEBUG
    dump_pointer("heap_sort2 done", root, nmemb);
#endif
}

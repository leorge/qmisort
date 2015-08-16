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

static char **first, **last, *key;
//size_t  anchor;

static void heap(size_t nmemb, size_t node)
{
#ifdef DEBUG
    dump_pointer("heap start", first, nmemb);
    qsort_called++;
#endif
    char    **child, **start = first + node, **parent = start;
    size_t  leaf;
    while(TRUE) {
        leaf = (node << 1) + 1;         // 2n + 1
        if (leaf >= nmemb) break;       // no child
        child = first + leaf;
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
    dump_pointer("heap done", first, nmemb);
#endif
}

void heap_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    size_t  n = nmemb;
    comp = compare;
    first = base;
    last = first + n - 1;  			// next element of the last element
    size_t node = (n - 1) >> 1;  	// lowest parent node
    do {    // build up a heap
        key = *(first + node);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "key = %s\n", dump_data(key));
#endif
        heap(n, node);
    } while (node--);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "A heap is built. compare = %ld\n", qsort_comp_str);
#endif
    do {    // sort
//        copy(key, last); copy(last, first); copy(first, key);   // *key <-- *last <-- *first <-- *key
    	key = *last; *last = *first; *first = key;	// swap first <--> last
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "key = %s\n", dump_data(key));
#endif
        last--; heap(--n, 0);     // shrink and re-build the heap
    } while (n);
#ifdef DEBUG
    dump_pointer("heap_sort done", first, nmemb);
#endif
}

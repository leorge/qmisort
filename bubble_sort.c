/*
 * bubble_sort.c
 *
 *	Bubble sort without swaps.
 *
 *  Created on: 2015/05/23
 *      Author: leo
 */

#include "sort.h"

static size_t   length;

/* pointer sorting */
void bubble_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    void *pivot;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT,
            "bubble_pointer(base=%p, nmemb=%ld, compare) start.\n", base, nmemb);
    qsort_called++;
#endif
    void **first = base;
    void **last = first + nmemb - 1;
    do {
        void **p1, **p2, **anchor = first + 1;
        for (p1 = first; (p2 = p1 + 1) <= last; p1++) {	// search a reverse order pair
        	if (compare(*p1, *p2) > 0) {	// found
                pivot = *p1; *p1 = *p2;		// save and shift first
        		for (p1 = p2; (p2 = p1 + 1) <= last; p1++) {	// search a greater element
        			if (compare(*p2, pivot) > 0) break;	// found
        			*p1 = *p2;	// shift first
        		}
        		*(anchor = p1) = pivot;	// restore
        	}
        }
        last = anchor - 1;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("bubble_pointer()", base, anchor - first);
#endif
    } while (first < last);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "bubble_pointer() done.\n");
#endif
}

/* array sorting */
static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s)\n", dump_data(dst), dump_data(src));
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

void bubble_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    char pivot[size];
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("bubble_sort() start.", base, nmemb, size);
    *pivot = 0;
#endif
    length = size;
    char *first = base;
    char *last = first + (nmemb - 1) * size;
    do {
        char *p1, *p2, *anchor = first + size;
#ifdef DEBUG
		qsort_called++;	// loop count
#endif
        for (p1 = first; (p2 = p1 + size) <= last; p1 += size) {	// search a reverse order pair
        	if (compare(p1, p2) > 0) {	// found
                copy(pivot, p1); copy(p1, p2);		// save and shift first
        		for (p1 = p2; (p2 = p1 + size) <= last; p1 += size) {	// search a greater element
        			if (compare(p2, pivot) > 0) break;	// found
        			copy(p1, p2);	// shift first
        		}
        		copy(anchor = p1, pivot);	// restore
        	}
        }
        last = anchor - size;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("bubble_array()", base, (anchor - first) / size, size);
#endif
    } while (first < last);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("bubble_sort() done.", base, nmemb, size);
#endif
}

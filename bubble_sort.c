/*
 * bubble.c
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
    void **top = base;
    void **bottom = top + nmemb - 1;
    do {
        void **p1, **p2, **anchor = top + 1;
        for (p1 = top; (p2 = p1 + 1) <= bottom; p1++) {	// search a reverse order pair
        	if (compare(*p1, *p2) > 0) {	// found
                pivot = *p1; *p1 = *p2;		// save and shift left
        		for (p1 = p2; (p2 = p1 + 1) <= bottom; p1++) {	// search a greater element
        			if (compare(*p2, pivot) > 0) break;	// found
        			*p1 = *p2;	// shift left
        		}
        		*(anchor = p1) = pivot;	// restore
        	}
        }
        bottom = anchor - 1;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("bubble_pointer()", base, anchor - top);
#endif
    } while (top < bottom);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pbubble_pointer() done.\n");
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
    qsort_called++;
    *pivot = 0;
#endif
    length = size;
    char *top = base;
    char *bottom = top + (nmemb - 1) * size;
    do {
        char *p1, *p2, *anchor = top + size;
        for (p1 = top; (p2 = p1 + size) <= bottom; p1 += size) {	// search a reverse order pair
        	if (compare(p1, p2) > 0) {	// found
                copy(pivot, p1); copy(p1, p2);		// save and shift left
        		for (p1 = p2; (p2 = p1 + size) <= bottom; p1 += size) {	// search a greater element
        			if (compare(p2, pivot) > 0) break;	// found
        			copy(p1, p2);	// shift left
        		}
        		copy(anchor = p1, pivot);	// restore
        	}
        }
        bottom = anchor - size;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("bubble_array()", base, (anchor - top) / size, size);
#endif
    } while (top < bottom);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("bubble_sort() done.", base, nmemb, size);
#endif
}

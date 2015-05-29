/*
 * cocktail_sort.c
 *
 *	Cocktail [shaker] sort, or bidirectional bubble sort without swaps.
 *
 *  Created on: 2015/05/23
 *      Author: leo
 */

#include "sort.h"

static size_t   length;

/* pointer sorting */
void cocktail_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    void *pivot;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT,
            "cocktail_pointer(base=%p, nmemb=%ld, compare) start.\n", base, nmemb);
#endif
    void **left = base;
    void **right = left + nmemb - 1;
    do {
        void **p1, **p2, **anchor = left + 1;
        for (p1 = left; (p2 = p1 + 1) <= right; p1++) {	// search a reverse order pair
        	if (compare(*p1, *p2) > 0) {	// found
                pivot = *p1; *p1 = *p2;		// save and shift left
        		for (p1 = p2; (p2 = p1 + 1) <= right; p1++) {	// search a greater element
        			if (compare(*p2, pivot) > 0) break;	// found
        			*p1 = *p2;	// shift left
        		}
        		*(anchor = p1) = pivot;	// restore
        	}
        }
        anchor = (right = anchor - 1) + 1;
        for (p2 = right; (p1 = p2 - 1) >= left; p2--) {	// search a reverse order pair
        	if (compare(*p1, *p2) > 0) {	// found
                pivot = *p2; *p2 = *p1;		// save and shift left
        		for (p2 = p1; (p1 = p2 - 1) >= left; p2--) {	// search a greater element
        			if (compare(*p1, pivot) < 0) break;	// found
        			*p2 = *p1;	// shift left
        		}
        		*(anchor = p2) = pivot;	// restore
        	}
        }
        left = anchor + 1;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("cocktail_pointer()", base, anchor - left);
#endif
    } while (left < right);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "cocktail_pointer() done.\n");
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

void cocktail_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    char pivot[size];
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("cocktail_sort() start.", base, nmemb, size);
    *pivot = 0;
#endif
    length = size;
    char *first = base;
    char *last = first + (nmemb - 1) * size;
    do {
    	// first to last
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
        // last to first
#ifdef DEBUG
        if (first < last) qsort_called++;	// loop count
#endif
        anchor = (last = anchor - size) - size;
        for (p2 = last; (p1 = p2 - size) >= first; p2 -= size) {	// search a reverse order pair
        	if (compare(p1, p2) > 0) {	// found
                copy(pivot, p2); copy(p2, p1);		// save and shift first
        		for (p2 = p1; (p1 = p2 - size) >= first; p2 -= size) {	// search a greater element
        			if (compare(p1, pivot) < 0) break;	// found
        			copy(p2, p1);	// shift first
        		}
        		copy(anchor = p2, pivot);	// restore
        	}
        }
        first = anchor + size;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("cocktail_array()", base, (anchor - first) / size, size);
#endif
    } while (first < last);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("cocktail_sort() done.", base, nmemb, size);
#endif
}

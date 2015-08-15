/*
 * insert_linear.c
 *
 *  Insertion sort with linear search.
 *
 *  Created on: 2013/01/08
 *      Author: leo
 */

#include "sort.h"

/* binary search */
void insert_binary(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("bins_pointer() start in " __FILE__, base, nmemb);
#endif
    void **hole = base;
    for (size_t idx = 1; idx < nmemb; idx++) {
#ifdef DEBUG
        qsort_called++;
#endif
        void *pivot = *++hole;
        // binary search
        int     chk;
        size_t  pos = 0, lo = 0, hi = idx - 1;
        while (lo <= hi) {
            chk = compare(pivot, base[pos = lo + ((hi - lo) >> 1)]);
            if (chk == 0) break; // found an equal element.
            else if (chk > 0) lo = pos + 1;
            else if (pos == 0) break;
            else hi = pos - 1;      // ck < 0
        }
        if (chk > 0) pos++;
        // store an element
        void **p1, **p2 = hole;;
        for (hi = idx; hi-- > pos; p2 = p1) *p2 = *(p1 = p2 - 1);
        *p2 = pivot;
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("bins_pointer() done.", base, nmemb);
#endif
}

/* linear search */
void insert_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("insert_sort() start in " __FILE__, base, nmemb);
#endif
    void **last = &base[nmemb - 1];     // point the last element
    for (void **hole = base + 1; hole <= last; hole++) {
#ifdef DEBUG
        qsort_called++;
#endif
    	void **p1, **p2, *pivot = *hole;	// make a hole
    	for (p2 = hole; p2 > base; p2 = p1) {
    		if (compare(*(p1 = p2 - 1), pivot) <= 0) break;
    		*p2 = *p1;
    	}
    	*p2 = pivot;
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("insert_sort() done.", base, nmemb);
#endif
}

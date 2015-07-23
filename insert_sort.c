/*
 * insert_linear.c
 *
 *  Insertion sort with linear search.
 *
 *  Created on: 2013/01/08
 *      Author: leo
 */

#include "sort.h"

/* pointer sorting */
void insert_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("insert_pointer() start in " __FILE__, base, nmemb);
    qsort_called++;
#endif
    void **last = &base[nmemb - 1];     // point the last element
    for (void **hole = base + 1; hole <= last; hole++) {
    	void **p1, **p2, *pivot = *hole;	// make a hole
    	for (p2 = hole; p2 > base; p2 = p1) {
    		if (compare(*(p1 = p2 - 1), pivot) <= 0) break;
    		*p2 = *p1;
    	}
    	*p2 = pivot;
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("insert_pointer() done.", base, nmemb);
#endif
}

/* array sorting */
static size_t   length;

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s)\n", dump_data(dst), dump_data(src));
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

void insert_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_linear() start.", base, nmemb, size);
#endif
    length = size;
#define first   ((char *)base)
    char *last = first + (nmemb -1) * size; // point the last element
    char pivot[size];
    for (char *hole = first + size; hole <= last; hole += size) {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) {
            char msg[20 + size]; sprintf(msg, "insert %s", hole);
            dump_array(msg, base, (hole - first) / size, size);
        }
        qsort_called++; // loop count
#endif
        copy(pivot, hole);	// make a hole
        char *p1, *p2;
        for (p2 = hole; p2 > first; p2 = p1) {
            if (compare(p1 = p2 - size, pivot) <= 0) break;
            copy(p2, p1);
        }
        if (p2 != hole) copy(p2, pivot);
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_linear() done.", base, nmemb, size);
#endif
}

/*
 * insert_linear.c
 *
 *  Insertion sort with linear search.
 *
 *  Created on: 2013/01/08
 *      Author: leo
 */

#include "sort.h"

static size_t   length;

/* array sorting */
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
    char pivot[size];
    for (size_t i = 1; i < nmemb; i++) {
        char    *p, *hole = first + i * size;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) {
        	char msg[20 + size]; sprintf(msg, "insert %s", hole);
        	dump_array(msg, base, i, size);
        }
        qsort_called++;	// loop count
#endif
        copy(pivot, hole);  // make a hole
        while (hole > first) {
            if (compare(p = hole - size, pivot) <= 0) break;
            copy(hole, p);  // move a hole
            hole = p;
        }
        copy(hole, pivot);  // bury the last hole
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_linear() done.", base, nmemb, size);
#endif
}

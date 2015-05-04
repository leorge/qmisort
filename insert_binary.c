/*
 * insert_binary.c
 *
 *  Insertion sort with binary search.
 *
 *  Created on: 2015/05/05
 *      Author: leo
 */

#include "sort.h"

static size_t   length;

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s)\n", dump_data(dst), dump_data(src));
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

/* in-place sort    */
void insert_binary(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_sort() start.", base, nmemb, size);
    qsort_called++;
#endif
    length = size;	// for copy()
#define first   ((char *)base)
    char pivot[size];
    size_t idx = 1;
    char *hole = first + size;
    for (; idx < nmemb; hole += size) {
        copy(pivot, hole);  // make a hole
        // binary search
        int     ck;
        size_t  pos = 0, lo = 0, hi = idx - 1;
        while (lo <= hi)
        {   // binary search
            ck = compare(pivot, first + size * (pos = lo + ((hi - lo) >> 1)));
            if (ck == 0) break; // found an equal element.
            else if (ck > 0) lo = pos + 1;
            else if (pos == 0) break;
            else hi = pos - 1;      // ck < 0
        };
        if (ck > 0) pos++;
        // store an element
        char *p = hole;;
        for (hi = idx++; hi-- != pos; p -= size) copy(p, p - size);
        copy(p, pivot);  // bury the last hole
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("sorted array", base, idx, size);
#endif
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_sort() done.", base, nmemb, size);
#endif
}

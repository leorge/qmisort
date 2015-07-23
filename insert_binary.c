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

/* Pointer sorting */
void bins_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("bins_pointer() start in " __FILE__, base, nmemb);
    qsort_called++;
#endif
    void **hole = base;
    for (size_t idx = 1; idx < nmemb; idx++) {
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

/* Array sorting */
void insert_binary(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_binary() start.", base, nmemb, size);
#endif
    length = size;	// for copy()
#define first   ((char *)base)
    char pivot[size];
    char *hole = first;
    for (size_t idx = 1; idx < nmemb; idx++) {
#ifdef DEBUG
        qsort_called++;	// correspond to recursive call
#endif
        copy(pivot, hole += size);  // make a hole
        // binary search
        int     chk;
        size_t  pos = 0, lo = 0, hi = idx - 1;
        while (lo <= hi) {
            chk = compare(pivot, first + size * (pos = lo + ((hi - lo) >> 1)));
            if (chk == 0) break; // found an equal element.
            else if (chk > 0) lo = pos + 1;
            else if (pos == 0) break;
            else hi = pos - 1;      // ck < 0
        }
        if (chk > 0) pos++;
        // store an element
        char *p1, *p2 = hole;;
        for (hi = idx; hi-- > pos; p2 = p1) copy(p2, p1 = p2 - size);
        if (p2 != pivot) copy(p2, pivot);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("sorted array", base, idx, size);
#endif
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_binary() done.", base, nmemb, size);
#endif
}

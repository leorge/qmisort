/*
 * insert_sort.c
 *
 *  Insertion sort.
 *
 *  Created on: 2013/01/08
 *      Author: leo
 */

#include "sort.h"

static size_t   length;

/* index sort   */
void insert_psort(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    if (trace_level >= TRACE_DUMP) dump_pointer("insert_psort() start.", idxtbl, nmemb);
#endif
    for (size_t i = 1; i < nmemb; i++) {
        size_t  hole = i, j;
         char *pivot = idxtbl[hole];
        while (hole > 0) {
            if (compare(idxtbl[j = hole - 1], pivot) <= 0) break;
            idxtbl[hole] = idxtbl[j];
            hole = j;
        }
        idxtbl[hole] = pivot;
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("insert_psort() done.", idxtbl, nmemb);
#endif
}

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s)\n", dump_data(dst), dump_data(src));
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

/* in-place sort    */
void insert_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_sort() start.", base, nmemb, size);
    qsort_called++;
#endif
    length = size;
#define first   ((char *)base)
    char pivot[size];
    for (size_t i = 1; i < nmemb; i++) {
        char    *p, *hole = first + i * size;
        copy(pivot, hole);  // make a hole
        while (hole > first) {
            if (compare(p = hole - size, pivot) <= 0) break;
            copy(hole, p);  // move a hole
            hole = p;
        }
        copy(hole, pivot);  // bury the last hole
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_sort() done.", base, nmemb, size);
#endif
    }
}

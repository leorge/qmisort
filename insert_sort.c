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

/* pointer sorting */
void insert_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("insert_pointer() start in " __FILE__, base, nmemb);
#endif
    void *pivot, **p1, **p2;
    for (size_t i = 1; i < nmemb; i++) {
        p2 = base + i;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) {
            fprintf(OUT, "insert %s", dump_data(*p2));
            dump_pointer("", base, i);
        }
#endif
        pivot = *p2;
        while (p2 > base) {
            if (compare(*(p1 = p2 - 1), pivot) <= 0) break;
            *p2 = *p1; p2 = p1;
        }
        *p2 = pivot;
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("insert_pointer() done.", base, nmemb);
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

void insert_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_linear() start.", base, nmemb, size);
#endif
    length = size;
#define first   ((char *)base)
    char pivot[size];
    for (size_t i = 1; i < nmemb; i++) {
        char    *p1, *p2 = first + i * size;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) {
            char msg[20 + size]; sprintf(msg, "insert %s", p2);
            dump_array(msg, base, i, size);
        }
        qsort_called++; // loop count
#endif
        copy(pivot, p2);
        while (p2 > first) {
            if (compare(p1 = p2 - size, pivot) <= 0) break;
            copy(p2, p1); p2 = p1;
        }
        copy(p2, pivot);
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("insert_linear() done.", base, nmemb, size);
#endif
}

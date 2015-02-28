/*
 * qsort_middle.c
 *
 *  K&R style Quicksort. pivot is a middle element.
 *
 *  Created on: 2013/02/09
 *      Author: leo
 */
#include "sort.h"

static int  (*comp)(const void *, const void *);
static size_t   length;
static char *swapbuf;
static char *pivot;

/* exchange 2 elements */
static void swap(void *p1, void *p2)
{
    if (p1 == p2) return;
#ifdef DEBUG
    qsort_moved += 3;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(%s, %s)\n", dump_data(p1), dump_data(p2));
#endif
    memcpy(swapbuf, p1, length);
    memcpy(p1, p2, length);
    memcpy(p2, swapbuf, length);
}

static void sort(void *base, size_t nmemb) {
    if (nmemb <= 1) return; // 0 or 1
#ifdef DEBUG
    qsort_called++;
    dump_array("sort() start in " __FILE__, base, nmemb, length);
#endif
#define first   ((char *)base)
    char *last = first + (nmemb - 1) * length;
    char *store = first;
    swap(first, first + (nmemb >> 1) * length); // middle element
    for (char *p = first + length; p <= last; p += length) {
        if (comp(p, first) < 0) swap(p, store += length);
    }
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP && first != store)
        fprintf(OUT, "restore pivot %s to %s at [%ld]\n",
            dump_data(first), dump_data(store), (store - first) / length);
#endif
    swap(first, store);
    size_t  n_lo = (store - first) / length;    // number of element in lower partition
    size_t  n_hi = (last - store) / length;
#ifdef DEBUG
    dump_array("sort() partitioned", first, nmemb, length);
    dump_rate(n_lo, n_hi);
#endif
    sort(first, n_lo);
    sort(store + length, n_hi);
#ifdef DEBUG
    dump_array("sort() done.", first, nmemb, length);
#endif
}

void qsort_middle(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
        char a[size]; swapbuf = a; *a = '\0';
        char b[size]; pivot = b; *b = '\0';
        length = size; comp = compare;
        sort(base, nmemb);
    }
}

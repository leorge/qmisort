/*
 * qsort_med3.c
 *
 *  Traditional quick sort using swap: pivot is a meidan of 3 quick sort.
 *
 *  Created on: 2013/02/04
 *      Author: leo
 */
#include "sort.h"

static int  (*comp)(const void *, const void *);
static size_t   length;
static char *swapbuf;

/* exchange 2 elements */
static void swap(void *p1, void *p2)
{
    if (p1 == p2) return;
#ifdef DEBUG
    qsort_moved += 3;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "swap(%s, %s)\n", dump_data(p1), dump_data(p2));
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
    char *middle = first + length * (nmemb >> 1);
    char *pivot = nmemb <= small_boundary ? middle :
    			 (comp(first, last) < 0 ?
                 (comp(middle, first) < 0 ? first: (comp(middle,  last) < 0 ? middle : last)) :
                 (comp(middle, last ) < 0 ? last : (comp(middle, first) < 0 ? middle : first)));
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot = %s\tfrom (%s, %s, %s)\n"
            , dump_data(pivot), dump_data(first), dump_data(middle), dump_data(last));
#endif
    swap(first, pivot);
    char *lo = pivot = first;
    char *hi = last + length;
    while (TRUE) {
        while(comp(lo += length, pivot) < 0) if (lo >= last) break;
        while(comp(hi -= length, pivot) > 0) if (hi <= first) break;
#ifdef  DEBUG
        if (trace_level >= TRACE_DEBUG) fprintf(OUT, "lo = %s\thi = %s\n"
                , dump_data(lo), dump_data(hi));
#endif
        if (lo >= hi) break;
        swap(lo, hi);
    }
    swap(first, hi);
#ifdef  DEBUG
    dump_array("sort() partitioned.", base, nmemb, length);
#endif
    size_t  n_lo = (hi - first) / length;   // number of elements in lower partition
    size_t  n_hi = (last - hi) / length;
#ifdef  DEBUG
    dump_rate(n_lo, n_hi);
#endif
    sort(first, n_lo);
    sort(hi + length, n_hi);
#ifdef DEBUG
    dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_med3(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
        char a[size]; swapbuf = a; *a = '\0';
        length = size; comp = compare;
        sort(base, nmemb);
    }
}

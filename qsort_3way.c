/*
 * qsort_3way.c
 *
 *  3-way partitioning quicksort : pivot is the first element.
 *
 *  Template is http://algs4.cs.princeton.edu/23quicksort/Quick3way.java.html
 *
 *  Created on: 2016/05/03
 *      Author: Takeuchi Leorge <qmisort@gmail.com>
 */
#include "sort.h"

static int  (*comp)(const void *, const void *);
static size_t   length;
static char *swapbuf;   // tricky

/* exchange 2 elements */
static void copy(void *p1, void *p2)
{
    if (p1 == p2) return;
#ifdef DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(%s, %s)\n", dump_data(p1), dump_data(p2));
#endif
    memcpy(p1, p2, length);
}

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
#define first   ((char *)base)
#ifdef DEBUG
    qsort_called++;
    dump_array("sort() start in " __FILE__, base, nmemb, 0, 0, length);
#endif
    char *last = base + (nmemb - 1) * length;
    char *lt = first, *p = first + length, *gt = last;
    char pivot[length]; copy(pivot, first);
    while (p <= gt) {
#ifdef  DEBUG
        if (trace_level >= TRACE_DEBUG) fprintf(OUT, "*p = %s\n", dump_data(p));
#endif
        int chk = comp(p, pivot);
        if (chk < 0) {
            swap(p, lt);
            lt += length; p += length;
        }
        else if (chk > 0) {
            swap(p, gt);
            gt -= length;
        }
        else
            p += length;
    }
    size_t  n_lo = (lt - first) / length;   // number of elements in lower partition
    size_t  n_hi = (last - gt) / length;
#ifdef  DEBUG
    dump_array("sort() partitioned.", base, n_lo, 1, n_hi, length);
    dump_rate(n_lo, n_hi);
#endif
    sort(first, n_lo);
    sort(gt + length, n_hi);
#ifdef DEBUG
    dump_array("sort() done.", base, nmemb, 0, 0, length);
#endif
}

void qsort_3way(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    char a[size]; swapbuf = a; *a = '\0';
    length = size; comp = compare;
    sort(base, nmemb);
}

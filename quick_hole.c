/*
 * qsort_hole.c
 *
 *  Quick sort - Use a last element as a hole.
 *
 *  Created on: 2013/01/01
 *      Author: leo
 */
#include "sort.h"

static int      (*comp)(const void *, const void *);
static size_t   length;
static char *pivot;

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s)\n", dump_data(dst), dump_data(src));
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

static void sort(void *base, size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_array("sort() start in " __FILE__, base, nmemb, length);
#endif
	char *first = base;
    char *last = first + length * (nmemb - 1);  // point a last element
    char *hole = last;
    copy(pivot, hole);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s [last]\n", dump_data(pivot));
#endif
    char *lo = first, *hi = hole - length;
    for (; lo < hole; lo += length) {
        if (comp(lo, pivot) > 0) {
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
            copy(hole, lo);
            hole = lo;
            for (; hi > hole; hi -= length) {
                if (comp(hi, pivot) < 0) {  // symmetric comparison
#ifdef  DEBUG
                    if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
                    copy(hole, hi);
                    hole = hi;
                }
            }
        }
    }
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
            dump_data(pivot), dump_data(hole), (hole - first) / length);
#endif
    copy(hole, pivot);  // restore
    size_t  n_lo = (hole - first) / length; // number of element in lower partition
    size_t  n_hi = (last - hole) / length;
#ifdef DEBUG
    dump_array("sort() partitioned", base, nmemb, length);
    dump_rate(n_lo, n_hi);
#endif
    sort(first, n_lo);
    sort(hole + length, n_hi);
#ifdef DEBUG
    dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_hole(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
        char a[size]; pivot = a; *a = '\0';
        length = size; comp = compare;
        sort(base, nmemb);
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "qsort_hole() done.\n");
#endif
    }
}

/* pointer sorting */

static void psort(void **base, size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_pointer("sort() start in " __FILE__, base, nmemb);
#endif
	void **first = base;
    void **last = first + (nmemb - 1);  // point a last element
    void **hole = last;
    pivot = *hole;
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s [last]\n", dump_data(pivot));
#endif
    void **lo = first, **hi = hole - 1;
    for (; lo < hole; lo++) {
        if (comp(*lo, pivot) > 0) {
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(*lo), dump_data(*hole));
#endif
            *hole = *lo; hole = lo;
            for (; hi > hole; hi--) {
                if (comp(*hi, pivot) < 0) {  // symmetric comparison
#ifdef  DEBUG
                    if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(*hole), dump_data(*hi));
#endif
                    *hole = *hi; hole = hi;
                }
            }
        }
    }
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
            dump_data(pivot), dump_data(*hole), hole - first);
#endif
    *hole = pivot;  // restore
    size_t  n_lo = hole - first; // number of element in lower partition
    size_t  n_hi = last - hole;
#ifdef DEBUG
    dump_pointer("sort() partitioned", base, nmemb);
    dump_rate(n_lo, n_hi);
#endif
    psort(first, n_lo);
    psort(hole + 1, n_hi);
#ifdef DEBUG
    dump_pointer("sort() done.", base, nmemb);
#endif
}

void qsort_phole(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("qsort_phole() start in " __FILE__, base, nmemb);
#endif
    comp = compare;
    psort(base, nmemb);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "qsort_phole() done.\n");
#endif
}

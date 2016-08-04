/*
 * quick_hybrid.c
 *
 *  Hybrid sort with Asymmetric Quicksort and another.
 *
 *  Created on: 2013/03/23
 *      Author: leo
 */
#include <math.h>
#include "sort.h"

static int      (*comp)(const void *, const void *);
static size_t   length;

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    dump_copy(dst, src);
#endif
    memcpy(dst, src, length);
}

static void sort(void *base, size_t nmemb) {
#ifdef  DEBUG
    void *init_base = base;
    size_t init_nmemb = nmemb;
    if (init_nmemb > 1) dump_array("sort() start in " __FILE__, init_base, init_nmemb, 0, 0, length);
#endif
    while (nmemb > threshold) {
#ifdef DEBUG
        qsort_called++;
        dump_array("sort() start in " __FILE__, base, nmemb, 0, 0, length);
#endif
        char *first = (char *)base, *last = first + (nmemb - 1) * length;
        char *hole = pivot_array(first, nmemb, length, ((size_t)log2(nmemb) >> 1) | 1, comp);
        char pivot[length]; copy(pivot, hole); copy(hole, last);    // pivot <-- hole <-- last
        char *lo = first,  *hi = (hole = last) - length, *eq = NULL;
        for (int chk; lo < hole; lo += length) {
            if ((chk = comp(lo, pivot)) >= 0) {
                if (chk > 0) eq = NULL; // discontinued
                else if (eq == NULL) eq = hole;
#ifdef  DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
                copy(hole, lo); hole = lo;
                for (; hi > hole; hi -= length) {
                    if ((chk =comp(hi, pivot)) < 0) {
#ifdef  DEBUG
                        if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
                        copy(hole, hi); hole = hi;
                    }
                    else if (chk > 0) eq = NULL;
                    else if (eq == NULL) eq = hi;   // first equal element
                }
            }
        }
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
            dump_data(pivot), dump_data(hole), (hole - first) / length);
#endif
        copy(hole, pivot);  // restore
        if (eq == NULL) eq = hole;
#ifdef DEBUG
        else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", (eq - hole) / length);
#endif
        // sort sub-arrays iterative and recursive.
        size_t  n_lo = (hole - first) / length; // number of element in lower partition
        size_t  n_hi = (last - eq) / length;
#ifdef DEBUG
        dump_array("sort() partitioned", base, nmemb, 0, 0, length);
        dump_rate(n_lo, n_hi);
#endif
        if (n_lo < n_hi) {
            sort(base, n_lo);  // sort shorter sub-array
            base = eq + length; nmemb = n_hi;
        } else {
            sort(eq + length, n_hi);
            nmemb = n_lo;
        }
    }
    (*medium_func)(base, nmemb, length, comp);
#ifdef DEBUG
    if (init_nmemb > 1) dump_array("sort() done.", init_base, init_nmemb, 0, 0, length);
#endif
}

void quick_hybrid(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
        length = size; comp = compare;
        sort(base, nmemb);
    }
}


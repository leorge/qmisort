/*
 * quick_pmiddle.c
 *
 *  Indirect Quicksort : a pivot is the middle element.
 *
 *  Created on: 2015/09/08
 *      Author: leo
 */

#include "sort.h"

static int  (*comp)(const void *, const void *);

static void sort(void **base, size_t nmemb) {
#ifdef DEBUG
    void *init_base = base;
    size_t init_nmemb = nmemb;
    if (init_nmemb > 1) dump_pointer("sort() start in " __FILE__, init_base, init_nmemb);
#endif
    while (nmemb > 1) {
#ifdef DEBUG
        qsort_called++;
        dump_pointer("sort() partition start", base, nmemb);
#endif
        void **first = base;
        void **last = first + (nmemb - 1);  // point the last element
        void **hole = first + (nmemb >> 1); // middle element
        char *pivot = *hole;        // save a pivot
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s [%ld]\n", dump_data(pivot), hole - first);
#endif
        *hole = *last; hole = last; // move the last element to the middle position.
        void **lo = first, **hi = last - 1, **eq = NULL;
        for (int chk; lo < hole; lo++) {
            if ((chk = comp(*lo, pivot)) >= 0) {
#ifdef  DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(*lo), dump_data(*hole));
#endif
                if (chk > 0) eq = NULL;
                else if (eq == NULL) eq = hole;
                *hole = *lo; hole = lo;
                for (; hi > hole; hi--) {
                    if ((chk = comp(*hi, pivot)) < 0) {  // symmetric comparison
#ifdef  DEBUG
                        if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(*hole), dump_data(*hi));
#endif
                        *hole = *hi; hole = hi; eq = NULL;
                    }
                    else if (chk > 0) eq = NULL;
                    else if (eq == NULL) eq = hi;   // first equal element
                }
            }
        }
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
                dump_data(pivot), dump_data(*hole), hole - first);
#endif
        *hole = pivot;  // restore
#ifdef DEBUG
        dump_pointer("sort() partitioned", base, nmemb);
#endif
        if (eq == NULL) eq = hole;
#ifdef DEBUG
        else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", eq - hole);
#endif
        size_t  n_lo = hole - first; // number of element in lower partition
        size_t  n_hi = last - eq;
#ifdef DEBUG
        dump_rate(n_lo, n_hi);
#endif
        if (n_lo < n_hi) {
            sort(first, n_lo);
            base = eq + 1; nmemb = n_hi;
        }
        else {
            sort(eq + 1, n_hi);
            nmemb = n_lo;
        }
    }
#ifdef DEBUG
    dump_pointer("sort() done.", base, nmemb);
#endif
}

void quick_pmiddle(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    dump_pointer("quick_pmiddle() start in " __FILE__, base, nmemb);
#endif
    comp = compare;
    sort(base, nmemb);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "quick_pmiddle() done.\n");
#endif
}

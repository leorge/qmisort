/*
 * quick_hole.c
 *
 *  Quicksort uses a last element as a pivot hole instead of swaps.
 *
 *  Created on: 2013/01/01
 *      Author: Takeuchi Leorge <qmisort@gmail.com>
 */
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
    if (nmemb > 1) {
#ifdef DEBUG
        qsort_called++;
        dump_array("sort() start in " __FILE__, base, nmemb, 0, 0, length);
#endif
#define first (char *)base
        char *last = first + (nmemb - 1) * length;  // point the last element
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s [last]\n", dump_data(last));
#endif
        char pivot[length], *hole; copy(pivot, hole = last);    // save the last element as a pivot
        char *lo = first, *hi = last - length;  // search pointers
        for (; lo < hole; lo += length) {       // outer loop
            if (comp(lo, pivot) > 0) {
#ifdef  DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
                copy(hole, lo); hole = lo;  // move a hole to lo.
                for (; hi > hole; hi -= length) {   // inner loop, symmetric to the outer loop
                    if (comp(hi, pivot) < 0) {      // symmetric comparison
#ifdef  DEBUG
                        if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
                        copy(hole, hi); hole = hi;  // move a hole to hi
                    }
                }
            }
        }
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%s]\n",
                dump_data(pivot), dump_data(hole), dump_size_t((hole - first) / length));
#endif
        copy(hole, pivot);  // restore
        size_t  n_lo = (hole - first) / length; // the number of elements in lower partition
        size_t  n_hi = (last - hole) / length;
#ifdef DEBUG
        dump_array("sort() partitioned.", base, n_lo, 1, n_hi, length);
        dump_rate(n_lo, n_hi);
#endif
        // recursive calls
        sort(first, n_lo);
        sort(hole + length, n_hi);
#ifdef DEBUG
        dump_array("sort() done.", base, nmemb, 0, 0, length);
#endif
    }
}

void quick_hole(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    length = size; comp = compare;
    sort(base, nmemb);
}

/*
 * quick_asymm.c : modified quick_secure.c
 *
 *  Asymmetric quick_secure() avoids an issue of many repeated elements
 *  because of asymmetric loops and exclusion of continuous equal elements.
 *  Pivot is the middle element, thus a sorted data sequence is not a worst case.
 *
 *  Test with a data sequence (3,1,2,2,2).
 *
 *  Created on: 2016/02/28
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
#ifdef  DEBUG
    void *init_base = base;
    size_t init_nmemb = nmemb;
    if (init_nmemb > 1) dump_array("sort() start in " __FILE__, init_base, init_nmemb, 0, 0, length);
#endif
    while (nmemb > 1) {
#ifdef DEBUG
        qsort_called++;
        dump_array("sort() partition start.", base, nmemb, 0, 0, length);
#endif
#define first (char *)base
        char *last = first + (nmemb - 1) * length;  // point a last element
        char *hole = first + (nmemb >> 1) * length; // choose the middle element as a pivot
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s [%s] <-- last = %s\n"
                , dump_data(hole), dump_size_t(NULL, (hole - first) / length), dump_data(last));
#endif
        char pivot[length]; copy(pivot, hole); copy(hole, last);    // pivot <-- hole <-- last
        char *lo = first,  *hi = (hole = last) - length, *eq = NULL;
        for (int chk; lo < hole; lo += length) {    // outer loop
            if ((chk = comp(lo, pivot)) >= 0) {
                if (chk > 0) eq = NULL; // equal elements discontinued
                else if (eq == NULL) eq = hole; // first equal element
#ifdef  DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
                copy(hole, lo); hole = lo;  // move a hole to lo.
                for (; hi > hole; hi -= length) {    // asymmetric loop to the outer loop
                    if ((chk = comp(hi, pivot)) < 0) {  // asymmetric comparison
#ifdef  DEBUG
                        if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
                        copy(hole, hi); hole = hi;  // move a hole to hi
                    }
                    else if (chk > 0) eq = NULL;
                    else if (eq == NULL) eq = hi;   // first equal element
                }
            }
        }
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%s]\n",
                dump_data(pivot), dump_data(hole), dump_size_t(NULL, (hole - first) / length));
#endif
        copy(hole, pivot);  // restore
        size_t  n_lo = (hole - first) / length; // the number of elements in lower partition
        if (eq == NULL) eq = hole;
        size_t  n_hi = (last - eq) / length;
#ifdef DEBUG
        dump_array("sort() partitioned.", base, n_lo, (eq - hole) / length + 1, n_hi, length);
        dump_rate(n_lo, n_hi);
#endif
        // recursion and iteration
        if (n_lo < n_hi) {
            sort(base, n_lo);   // shorter sub-array consists of lesser elements
            nmemb = n_hi; base = eq + length;    // prepare to the next loop
        } else {    // n_lo >= n_hi
            sort(eq + length, n_hi); // greater or equal elements
            nmemb = n_lo;
        }
    }
#ifdef DEBUG
    if (init_nmemb > 1) dump_array("sort() done.", init_base, init_nmemb, 0, 0, length);
#endif
}

void quick_asymm(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (base == NULL || compare == NULL || size == 0) return;
    length = size; comp = compare;
    sort(base, nmemb);
}

/*
 * quick_pivot.c : modified quick_random.c
 *
 *  Select the median of several random elements as a pivot.
 *
 *  Created on: 2013/05/10
 *      Author: Takeuchi Leorge <qmisort@gmail.com>
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
    while (nmemb > 1) {
#ifdef DEBUG
        qsort_called++;
        dump_array("sort() partition start.", base, nmemb, 0, 0, length);
#endif

#define first (char *)base
        char    *hole = NULL;
        if (nmemb <= single1) {  // middle element
            hole = first + (nmemb >> 1) * length;
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot is at middle %s [%ld]\n",
                    dump_data(hole), (hole - first) / length);
#endif
        }
        else if (nmemb <= median1) {  // choose a random element
            hole = first + (nmemb * rand() / RAND_BASE) * length;
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot is at random %s [%ld]\n",
                    dump_data(hole), (hole - first) / length);
#endif
        }
        else if (nmemb <= median3) {    // median of 3
            char *p1, *p2, *p3;
            p1 = first + (((nmemb >> 1) * rand()) / RAND_BASE) * length;  // in [0, N/2)
            size_t distance = (nmemb >> 2) * length;    // N / 4
            p3 = (p2 = p1 + distance) + distance;       // [N/4, 3N/4), [N/2, N)
            hole = (comp(p1, p3) < 0 ?
                   (comp(p2, p1) < 0 ? p1: (comp(p2, p3) < 0 ? p2 : p3)) :
                   (comp(p2, p3) < 0 ? p3: (comp(p2, p1) < 0 ? p2 : p1)));
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT,
                "Median of 3 in %ld from %s %s %s --> %s\n",
                nmemb, dump_data(p1), dump_data(p2), dump_data(p3), dump_data(hole));
            int LEFT = 0, RIGHT=0, CHK;
#define SIDE(a) CHK = comp((a), hole); if (CHK < 0) LEFT++; else if (CHK > 0) RIGHT++;
            SIDE(p1); SIDE(p2); SIDE(p3);
            assert(LEFT < 2 && RIGHT < 2);
#endif
        }
        else if (nmemb <= median5) {
            hole = median_of_5(base, nmemb, length, comp);
        }
        else if (nmemb <= medianL) {    // (log2(N)-1)|1
            hole = pivot_array(first, nmemb, length, ((size_t)log2(nmemb) - 1) | 1, comp);
        }
        else {  // log2(sqrt(N))|1
            hole = pivot_array(first, nmemb, length, ((size_t)log2(nmemb) >> 1) | 1, comp);
        }
        assert(hole != NULL);
        char *last = first + (nmemb - 1) * length;  // point a last element
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s [%ld] <-- last = %s\n"
                , dump_data(hole), (hole - first) / length ,dump_data(last));
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
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
                dump_data(pivot), dump_data(hole), (hole - first) / length);
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

void quick_pivot(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    length = size; comp = compare;
    sort(base, nmemb);
}

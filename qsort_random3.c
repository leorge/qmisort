/*
 * qsort_random3.c
 *
 *  Quick sort - Quick sort with a hole. Pivot is median of random 3 elements.
 *
 *  Created on: 2015/03/20
 *      Author: leo
 */
#include "sort.h"
#include <math.h>

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
#define first   ((char *)base)
    char    *hole, *last = first + length * (nmemb - 1);    // point a last element
    if (nmemb <= medium_boundary) {
        hole = first + length * (nmemb >> 1);  // middle element
    }
    else {
        size_t  distance = (size_t)(nmemb / 3);      // distance of elements
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "nmemb = %ld\tdistance = %ld\n" , nmemb, distance);
#endif
        char *p1 = first + (size_t)(random_number * distance) * length;  // 1st pick up point
        distance *= length;     // size in byte
        char *p2 = p1 + distance;
        char *p3 = p2 + distance;
#ifdef DEBUG
#endif
		hole = (comp(p1, p3) < 0
             ? (comp(p2, p1) < 0 ? p1: (comp(p2,  p3) < 0 ? p2 : p3))
             : (comp(p2, p3 ) < 0 ? p3 : (comp(p2, p1) < 0 ? p2 : p1)));
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "p1 = %s  p2 = %s  p3 = %s --> hole = %s\n",
        		dump_data(p1), dump_data(p2), dump_data(p3), dump_data(hole));
#endif
    }
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s <-- last = %s\n", dump_data(hole), dump_data(last));
#endif
    copy(pivot, hole);
    copy(hole, last);   // pivot <-- hole <-- last
    hole = last;
    char *lo = first, *hi = last - length, *eq = NULL;
    for (; lo < hole; lo += length) {
        if (comp(lo, pivot) >= 0) {
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
            copy(hole, lo);
            hole = lo;
            for (; hi > hole; hi -= length) {
                int chk;
                if ((chk = comp(hi, pivot)) < 0) {
#ifdef  DEBUG
                    if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
                    copy(hole, hi);
                    hole = hi;
                    eq = NULL;  // not equal then reset
                }
                else if (chk > 0) eq = NULL;
                else if (eq == NULL) eq = hi;   // first equal element
            }
        }
    }
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s --> %s [%ld]\n",
            dump_data(pivot), dump_data(hole), (hole - first) / length);
#endif
    copy(hole, pivot);  // restore
#ifdef DEBUG
    dump_array("sort() partitioned", base, nmemb, length);
#endif
    if (eq == NULL) eq = hole;
#ifdef DEBUG
    else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", (eq - hole) / length);
#endif
    size_t  n_lo = (hole - first) / length; // number of element in lower partition
    size_t  n_hi = (last - eq) / length;
#ifdef DEBUG
    dump_rate(n_lo, n_hi);
#endif
    sort(first, n_lo);
    sort(eq + length, n_hi);
#ifdef DEBUG
    dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_random3(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
		char a[size]; pivot = a; *a = '\0';
		length = size; comp = compare;
		set_random();
		sort(base, nmemb);
    }
}

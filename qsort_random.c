/*
 * qsort_random.c
 *
 *  Quick sort - select a random pivot as a hole.
 *               asymmetric search to resolve a few data issue.
 *
 *  Created on: 2013/05/10
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

static void sort(void *base, size_t nmemb, int depth) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_array("sort() start in " __FILE__, base, nmemb, length);
#endif
#define first   ((char *)base)
    char    *hole, *last = first + length * (nmemb - 1);    // point a last element
    size_t   random;
    if (depth > 0) {
		random = set_random();
        depth--;
    }
	else random = RAND_BASE >> 1;
    hole = first + (nmemb * random / RAND_BASE) * length;    // 0 .. (nmemb - 1)
    copy(pivot, hole);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) {
        fprintf(OUT,"pivot <-- %s [%ld]\n", dump_data(pivot), (hole - first) / length);
    }
#endif
    copy(hole, last);
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
                if ((chk =comp(hi, pivot)) < 0) {
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
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s --> %s [%ld]\n"
            , dump_data(pivot), dump_data(hole), (lo - first) / length);
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
    sort(first, n_lo, depth);
    sort(eq + length, n_hi, depth);
#ifdef DEBUG
    dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_random(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
        char a[size]; pivot = a; *a = '\0';
        length = size; comp = compare;
        sort(base, nmemb, random_depth);
    }
}

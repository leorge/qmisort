/*
 * quick_random.c
 *
 *  Quicksort - select a random pivot as a hole.
 *               asymmetric search to resolve a few data issue.
 *
 *  Created on: 2013/05/10
 *      Author: leo
 */
#include "sort.h"

static int      (*comp)(const void *, const void *);
static size_t   length;
static char *pivot;
#ifdef  DEBUG
static size_t   random;         // random number to reuse
#endif

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
#else
    size_t   random;
#endif
#define first   ((char *)base)
    char    *hole;
    if (nmemb <= small_boundary) {
        hole = first + (nmemb >> 1) * length;	// middle element
    }
    else {  // N is large
		if (depth > 0) {
			depth--;
			random = set_random();
		}
#ifdef DEBUG
		else if (reuse_random) {}   // no change
#endif
		else random = RAND_BASE >> 1;
        size_t distance;
        switch (pivot_type) {   // Quicksort Algorithm
        case PIVOT_RANDOM:      // a single element at random
            hole = first + (nmemb * random / RAND_BASE) * length;    // 0 .. (nmemb - 1)
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "random pivot is %s [%ld]\n",
                    dump_data(hole), (hole - first) / length);
#endif
            break;
        case PIVOT_RANDOM3:     // median of random 3 elements
            distance = nmemb / 3;    // distance between elements
            char *p1 = first + (distance * random / RAND_BASE) * length;
            char *p2 = p1 + (distance *= length);
            char *p3 = p2 + distance;
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT,
                    "random 3 : distance = %ld\t pickup = (%s, %s, %s)\n", distance / length, p1, p2, p3);
#endif
            hole = (comp(p1, p3) < 0 ?
                   (comp(p2, p1) < 0 ? p1: (comp(p2, p3) < 0 ? p2 : p3)) :
                   (comp(p2, p3) < 0 ? p3: (comp(p2, p1) < 0 ? p2 : p1)));
            break;
        case PIVOT_VARIOUS:     // median of variout elements
            hole = pivot_array(base, nmemb, length, pivot_number, comp, random);
            break;
        case PIVOT_LOG2:        // median of log2(N) elements
            hole = pivot_array(base, nmemb, length, ((size_t)log2(nmemb) - 1) | 1, comp, random);
            break;
        default:
#ifdef DEBUG
            assert(FALSE);
#endif
            break;
        }
    }
    char *last = first + length * (nmemb - 1);    // point a last element
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s [%ld] <-- last = %s\n"
    		, dump_data(hole), (hole - first) / length ,dump_data(last));
#endif
    copy(pivot, hole); copy(hole, last);    // pivot <-- hole <-- last
    char    *lo = first,  *hi = (hole = last) - length, *eq = NULL;
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

void quick_random(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
#ifdef DEBUG
        dump_array("quick_random() start in " __FILE__, base, nmemb, size);
#endif
        char a[size]; pivot = a; *a = '\0';
        length = size; comp = compare;
        sort(base, nmemb, random_depth);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "quick_random() done.\n");
#endif
    }
}

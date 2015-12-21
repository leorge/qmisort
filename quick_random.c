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
#ifdef  DEBUG
static size_t   random; // reused in DEBUG program
#endif

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    dump_copy(dst, src);
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

static void sort(void *base, size_t nmemb, int depth) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_array("sort() start in " __FILE__, base, nmemb, length);
#else
    size_t   random;    // not reused in Release program
#endif
    if (depth > 0) {
        depth--;
        random = set_random();  // to choose a random element
    }
#ifdef DEBUG
    else if (reuse_random) {}   // no change
#endif
    else random = RAND_BASE >> 1;   // to choose the middle element

#define first   ((char *)base)
    char    *hole;
    size_t distance;
    if (nmemb <= pivot1) {  // prior to pivot_type
        hole = first + (nmemb >> 1) * length;   // middle element
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot is at middle %s [%ld]\n",
                dump_data(hole), (hole - first) / length);
#endif
    }
    else if (nmemb <= pivot3) {
        distance = nmemb >> 2;    // N / 4
        char *p1 = first + ((distance >> 1) + ((distance * random) / RAND_BASE)) * length;  // in [N/8, 3N/8)
        char *p2 = p1 + (distance *= length);                                               // in [3N/8, 5N/8)
        char *p3 = p2 + distance;                                                           // in [5N/8, 7N/8)
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "Median of 3 in %ld from %s %s %s",
            nmemb, dump_data(p1), dump_data(p2), dump_data(p3));
#endif
        hole = (comp(p1, p3) < 0 ?
               (comp(p2, p1) < 0 ? p1: (comp(p2, p3) < 0 ? p2 : p3)) :
               (comp(p2, p3) < 0 ? p3: (comp(p2, p1) < 0 ? p2 : p1)));
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, " --> %s\n", dump_data(hole));
        int LEFT = 0, RIGHT=0, CHK;
#define SIDE(a) CHK = comp((a), hole); if (CHK < 0) LEFT++; else if (CHK > 0) RIGHT++;
        SIDE(p1); SIDE(p2); SIDE(p3);
        assert(LEFT < 2 && RIGHT < 2);
#endif
    }
    else if (nmemb <= pivot5) {
        hole = median5(base, nmemb, length, comp, random);
    }
    else {  // N is large
        hole = pivot_array(base, nmemb, length, ((size_t)log2(nmemb) - 1) | 1, comp, random);
    }
    char *last = first + length * (nmemb - 1);    // point a last element
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s [%ld] <-- last = %s\n"
            , dump_data(hole), (hole - first) / length ,dump_data(last));
#endif
    char pivot[length]; copy(pivot, hole); copy(hole, last);    // pivot <-- hole <-- last
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
        length = size; comp = compare;
        sort(base, nmemb, random_depth);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "quick_random() done.\n");
#endif
    }
}

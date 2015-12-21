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

typedef struct {
	char  *base;
	size_t N;
} ARRAY;

void dump_partition(const char *msg, ARRAY array) {
	dump_array(msg, array.base, array.N, length);
}

static void partition(const ARRAY array, ARRAY *sub_array, char *hole) {
    size_t nmemb = array.N;
    char *first = array.base;
    char *last = first + length * (nmemb - 1);    // point a last element
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s [%ld] <-- last = %s\n"
            , dump_data(hole), (hole - first) / length ,dump_data(last));
#endif
    char save[length]; copy(save, hole); copy(hole, last);    // save <-- hole <-- last
    char    *lo = first,  *hi = (hole = last) - length, *eq = NULL;
    for (; lo < hole; lo += length) {
        if (comp(lo, save) >= 0) {
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
            copy(hole, lo);
            hole = lo;
            for (; hi > hole; hi -= length) {
                int chk;
                if ((chk =comp(hi, save)) < 0) {
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
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore save %s --> %s [%ld]\n"
            , dump_data(save), dump_data(hole), (lo - first) / length);
#endif
    copy(hole, save);  // restore
#ifdef DEBUG
    dump_partition("sort() partitioned", array);
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
    sub_array[0].base = array.base; sub_array[0].N = n_lo;
    sub_array[1].base = eq;         sub_array[1].N = n_hi;
}

static void sort(ARRAY array, int depth) {
    if (array.N <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_partition("sort() start in " __FILE__, array);
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

    size_t distance, nmemb = array.N;
    char *hole, *first = array.base;
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
        hole = median5(first, nmemb, length, comp, random);
    }
    else {  // N is large
        hole = pivot_array(first, nmemb, length, ((size_t)log2(nmemb) - 1) | 1, comp, random);
    }

    ARRAY sub_array[2];
    partition(array, sub_array, hole);
    sort(sub_array[0], depth);
    sort(sub_array[1], depth);
#ifdef DEBUG
    dump_partition("sort() done.", array);
#endif
}

void quick_random(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
#ifdef DEBUG
        dump_array("quick_random() start in " __FILE__, base, nmemb, size);
#endif
        length = size; comp = compare;
        ARRAY array; array.base = base; array.N = nmemb;
        sort(base, nmemb, random_depth);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "quick_random() done.\n");
#endif
    }
}

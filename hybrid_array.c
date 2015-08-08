/*
 * array_sort.c
 *
 *  Array sort of QM / QMI sort
 *
 *  Created on: 2013/03/23
 *      Author: leo
 */
#include    <math.h>
#include "sort.h"

static int      (*comp)(const void *, const void *);
static size_t   length;
static char     *pivot;
#ifdef  DEBUG
static size_t   random;         // random number
#endif

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s)\n", dump_data(dst), dump_data(src));
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

static void sort(void *base, size_t nmemb, RANDOM_DEPTH depth) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_array("sort() start in " __FILE__, base, nmemb, length);
#else
    size_t   random;
#endif
#define first   ((char *)base)
    char *hole;
    if (nmemb <= small_boundary) {  // Trick to investigate entire quick sort
        hole = first + length * (nmemb >> 1);  // middle element
    }
    else if (nmemb <= medium_boundary) {    // hybrid sorting
        medium_func(base, nmemb, length, comp);
        return;
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
        case PIVOT_RANDOM:
            hole = first + (nmemb * random / RAND_BASE) * length;  // pick up one element at random
            break;
        case PIVOT_RANDOM3:
            distance = nmemb / 3;    // distance between elements
            char *p1 = first + (distance * random / RAND_BASE) * length;  // pick up median of random 3 elements
            char *p2 = p1 + (distance *= length);
            char *p3 = p2 + distance;
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT,
                    "nmemb = %ld\tdistance = %ld\t pickup = (%s, %s, %s)\n", nmemb, distance, p1, p2, p3);
#endif
            hole = (comp(p1, p3) < 0 ?
                   (comp(p2, p1) < 0 ? p1: (comp(p2, p3) < 0 ? p2 : p3)) :
                   (comp(p2, p3) < 0 ? p3: (comp(p2, p1) < 0 ? p2 : p1)));
            break;
        case PIVOT_VARIOUS:
            hole = pivot_array(base, nmemb, length, pivot_number, comp, random);
            break;
        case PIVOT_LOG2:
            hole = pivot_array(base, nmemb, length, ((size_t)log2(nmemb) - 1) | 1, comp, random);
            break;
        default:
#ifdef DEBUG
            assert(FALSE);
#endif
            break;
        }
    }
    char *last = first + length * (nmemb - 1);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s <-- last = %s\n", dump_data(hole), dump_data(last));
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
                if ((chk = comp(hi, pivot)) < 0) {
#ifdef  DEBUG
                    if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
                    copy(hole, hi);
                    hole = hi;
                    eq = NULL;
                }
                else if (chk > 0) eq = NULL;
                else if (eq == NULL) eq = hi;
            }
        }
    }
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
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
    sort(first, n_lo, depth);
    sort(eq + length, n_hi, depth);
#ifdef DEBUG
    dump_array("sort() done.", base, nmemb, length);
#endif
}

void hybrid_array(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
        char a[size]; pivot = a; *a = '\0';
        length = size; comp = compare;
        sort(base, nmemb, random_depth);
#ifdef DEBUG
        if (search_pivot && trace_level >= TRACE_DUMP)
            fprintf(OUT, "search_pivot = %ld times\n", search_pivot);
#endif
    }
}

/* Stable sort for pointer array */

typedef struct {
    size_t  index;      // index in a void **idxtbl
    void    *address;   // addres of element
} POINTER_INDEX;

static int      (*comp_p)(const void *, const void *);

static int acomp(const void *p1, const void *p2) {
    return  comp_p(((const POINTER_INDEX *)p1)->address, ((const POINTER_INDEX *)p2)->address);
}

static int icomp(const void *p1, const void *p2) {
    return  ((POINTER_INDEX *)p1)->index -  ((POINTER_INDEX *)p2)->index;
}

void stable_pointer(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *))
{   // like index_sort()
    if (nmemb <= 1) return;
    void *tbl = calloc(nmemb, sizeof(POINTER_INDEX));
    if (tbl != NULL) {
        /*  store idxtbl to POINTER_INDEX[] */
        void    **p = idxtbl;
        POINTER_INDEX   *t = (POINTER_INDEX *)tbl;
        for (size_t i = 0; i < nmemb; t++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%ld].address = %p %s\n", i, *p, (char *)*p);
#endif
            t->index = i++;
            t->address = *p++;  // may be gotten by malloc()
        }
        /*  sort    */
        char a[sizeof(POINTER_INDEX)]; pivot = a; *a = '\0';
        comp = acomp;       // sort() calls comp()
        comp_p = compare;   // comp() calls compare()
        length = sizeof(POINTER_INDEX);
        sort(tbl, nmemb, random_depth);   // sort array
        /*  sort to be stable and reoder idxtbl */
        comp = icomp;
        t = (POINTER_INDEX *)tbl;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[0] = %ld, %p %s\n", t->index, t->address, (char *)(t->address));
#endif
        POINTER_INDEX   *from = t++;
        for (size_t i = 1; i < nmemb; i++, t++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%ld] = %ld, %p %s\n", i, t->index, t->address, (char *)(t->address));
#endif
            if (compare(t->address, from->address)) {
                sort(from, t - from, random_depth);
                from = t;
            }
        }
        sort(from, t - from, 0);
        /* reorder idxtbl   */
        p = idxtbl;
        t = (POINTER_INDEX *)tbl;
        for (size_t i = 0; i < nmemb; i++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%ld] = %ld, %p %s\n", i, t->index, t->address, (char *)(t->address));
#endif
            *p++ = t++->address;
        }
        /* done */
        free(tbl);
    }
}

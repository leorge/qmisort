/*
 * index_sort.c
 *
 *  Index sort of QM / QMI sort.
 *
 *  Created on: 2013/03/27
 *      Author: leo
 */
#include    <math.h>
#include "sort.h"

static int  (*comp)(const void *, const void *);
static int  comp_idx(const void *p1, const void *p2) {
#ifdef DEBUG
    if (trace_level >= TRACE_COMPARE) fprintf(OUT, "comp_idx(%s, %s)\n",
        dump_data(*(char * const *)p1), dump_data(*(char * const *)p2));
#endif
    return  comp(*(char * const *)p1, *(char * const *)p2);
}


#ifdef  DEBUG
static  size_t  search_pivot;
#endif

static void sort(void *base[], size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++; // Counted in middle_index() or pivot_sort()
    dump_pointer("sort() start in " __FILE__, base, nmemb);
#endif
    if (nmemb <= medium_boundary) {
        (*medium_func)(base, nmemb, sizeof(void *), comp_idx);
    }
    else {
        size_t distance = ((size_t)log2(nmemb) - 1) | 1;
        void **hole = pivot_pointer(base, nmemb, distance, comp);
        void **last = &base[nmemb - 1];
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "\npivot <-- pivot = %s <-- last = %s\n", dump_data(*hole), dump_data(*last));
#endif
        void    *pivot = *hole; *hole = *last;  // *pivot <-- *hole <-- *last  cf. sort() in array_sort.c
        void    **lo, **hi = hole = last, **eq = NULL;
        for (hi--, lo = base; lo < hole; lo++) {
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "start : lo=%p\thole=%p\thi=%p\n", lo, hole, hi);
#endif
            if (comp(*lo, pivot) >= 0) {
#ifdef  DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(*lo), dump_data(*hole));
#endif
                *hole = *lo;    // copy address of bigger element
                hole = lo;      // change position to store
                for (; hi > hole; hi--) {
                    int chk;
                    if ((chk = comp(*hi, pivot)) < 0) {
#ifdef  DEBUG
                        if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(*hole), dump_data(*hi));
#endif
                        *hole = *hi;
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
                dump_data(pivot), dump_data(*hole), hole - base);
#endif
        *hole = pivot;  // restore
#ifdef DEBUG
        dump_pointer("sort() partitioned", base, nmemb);
#endif
        if (eq == NULL) eq = hole;
#ifdef DEBUG
        else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", eq - hole);
#endif
        size_t  n_lo = hole - base; // number of element in lower partition
        size_t  n_hi = last - eq;
#ifdef DEBUG
        dump_rate(n_lo, n_hi);
#endif
        sort(base, n_lo);
        sort(eq + 1, n_hi);
    }
#ifdef DEBUG
    dump_pointer("sort() done.", base, nmemb);
#endif
}

void hybrid_pointer(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    if (idxtbl != NULL) {
        comp = compare;
        sort(idxtbl, nmemb);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) {
            fprintf(OUT, "search pivot %ld times\n", search_pivot);
        }
#endif
    }
}

void hybrid_index(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    void **idxtbl = make_index(base, nmemb, size);
    if (idxtbl != NULL) {
        hybrid_pointer(idxtbl, nmemb, compare);
        unindex(base, idxtbl, nmemb, size);
        free(idxtbl);
    }
}

/***    Stable sort for continuous array    ***/
static int acomp(const void *p1, const void *p2) {
    return  p1 - p2;
}

static void address_sort(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *))
{
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) {
        fprintf(OUT, "address_sort() start. nmemb = %ld\n", nmemb);
        for (int i = 0; i < nmemb; i++) fprintf(OUT, "idxtbl[%d] = %p\t%s\n", i, idxtbl[i], (char *)idxtbl[i]);
    }
#endif
    comp = acomp;   // replace function to compare
    void    **p = idxtbl;
    void    **from = p++;
    for (int i = 1; i < nmemb; i++, p++) {
        if (compare(*p, *from)) {
            sort(from, p - from);
            from = p;
        }
    }
    sort(from, p - from);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) {
//      for (int i = 0; i < nmemb; i++) fprintf(OUT, "idxtbl[%d] = %p\t%s\n", i, idxtbl[i], (char *)idxtbl[i]);
        fprintf(OUT, "address_sort() done.\n");
    }
#endif
}

void stable_array(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    void **idxtbl = make_index(base, nmemb, size);
    if (idxtbl != NULL) {
        hybrid_pointer(idxtbl, nmemb, compare);
        address_sort(idxtbl, nmemb, compare);
        unindex(base, idxtbl, nmemb, size);
        free(idxtbl);
    }
}

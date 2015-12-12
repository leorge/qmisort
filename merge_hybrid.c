/*
 * merge_insert.c
 *
 *  hybrid of merge sort and conventional insertion sort. Stable
 *
 *  Created on: 2015/02/04
 *      Author: leo
 */

#include "sort.h"

static int  (*comp)(const void *, const void *);

static void sort(void **dst, void **src, size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_pointer("sort() start in " __FILE__, src, nmemb);
    void **first = dst;
#endif
    if (nmemb <= small_boundary) {    /* Insertion sort */
        small_func(dst, nmemb, comp);
    }
    else {
        size_t n_lo = nmemb >> 1;   // = nmemb / 2
        size_t n_hi = nmemb - n_lo;
        sort(src, dst, n_lo);
        sort(&src[n_lo], &dst[n_lo], n_hi);
        void **left = src;
        void **right = &left[n_lo];
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) {
            dump_pointer("left", left, n_lo);
            dump_pointer("right", right, nmemb - n_lo);
        }
#endif
        while (TRUE) {
            if (comp(*left, *right) <= 0) {
#ifdef DEBUG
                if (trace_level >= TRACE_MOVE) fprintf(OUT, "merge %s\n", dump_data(*left));
#endif
                *dst++ = *left++;     // add one
                if (--n_lo <= 0) {  // empty?
#ifdef DEBUG
                	if (trace_level >= TRACE_MOVE) dump_pointer("append", right, n_hi);
#endif
                    memcpy(dst, right, n_hi * sizeof(void *));    // append remained data
                    break;
                }
            }
            else {
#ifdef DEBUG
                if (trace_level >= TRACE_MOVE) fprintf(OUT, "merge %s\n", dump_data(*right));
#endif
                *dst++ = *right++;
                if (--n_hi <= 0) {
#ifdef DEBUG
                	if (trace_level >= TRACE_MOVE) dump_pointer("append", left, n_lo);
#endif
                    memcpy(dst, left, n_lo * sizeof(void *));
                    break;
                }
            }
        }
    }
#ifdef DEBUG
    dump_pointer("sort() done.", first, nmemb);
#endif
}

// pointer sort
void merge_phybrid(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb > 1) {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT,
                "merge_phybrid(base=%p, nmemb=%ld, compare) start.\n", base, nmemb);
#endif
        void **idxtbl = calloc(nmemb, sizeof(void *));  // double buffer
        if (idxtbl == NULL) perror(NULL);
        else {
            comp = compare;
            memcpy(idxtbl, base, nmemb * sizeof(void *));   // copy pointers
            sort(base, idxtbl, nmemb);
            free(idxtbl);
        }
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "merge_phybrid() done.\n");
#endif
    }
}

// index sort
void merge_hybrid(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb > 1) {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT,
                "merge_hybrid(base=%p, nmemb=%ld, size=%ld, compare) start.\n", base, size, nmemb);
#endif
        void **idxtbl = make_index(base, nmemb, size);
        if (idxtbl != NULL) {
            merge_phybrid(idxtbl, nmemb, compare);
            unindex(base, idxtbl, nmemb, size);
            free(idxtbl);
        }
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "merge_hybrid() done.\n");
#endif
    }
}

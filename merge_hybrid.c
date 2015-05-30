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

static void sort(void **dst, void **src, bool revert, size_t nmemb) {
    if (nmemb <= 1) return;
    void **store = revert ? src : dst;  // destination
#ifdef DEBUG
    qsort_called++;
    if (trace_level >= TRACE_DUMP) dump_pointer("sort() start in " __FILE__, revert ? dst : src, nmemb);
    void **first = store;
#endif
    if (nmemb <= MAX_SIZE) {    /* Insertion sort */
        for (size_t idx = 1; idx < nmemb; idx++) {
            void *pivot = store[idx];	// save an address
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "insert %s\n", dump_data(pivot));
#endif
            // linear search
            void **p = &store[idx], **q = p;
            do {
                if (comp(*--p, pivot) <= 0) break;
                *q-- = *p;  // slide an element
            } while (p != store);
            *q = pivot;	// restore an address
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) dump_pointer("sorted ", store, idx + 1);
#endif
        }
    }
    else {
        size_t n_lo = nmemb >> 1;   // = nmemb / 2
        size_t n_hi = nmemb - n_lo;
        sort(dst, src, ! revert, n_lo);
        sort(&dst[n_lo], &src[n_lo], ! revert, n_hi);
        void **left = revert ? dst : src;
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
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "add %s\n", dump_data(*left));
#endif
                *store++ = *left++;     // add one
                if (--n_lo <= 0) {  // empty?
#ifdef DEBUG
                    if (trace_level >= TRACE_DUMP) dump_pointer("append", right, n_hi);
#endif
                    memcpy(store, right, n_hi * sizeof(void *));    // append remained data
                    break;
                }
            }
            else {
#ifdef DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "add %s\n", dump_data(*right));
#endif
                *store++ = *right++;
                if (--n_hi <= 0) {
#ifdef DEBUG
                    if (trace_level >= TRACE_DUMP) dump_pointer("append", left, n_lo);
#endif
                    memcpy(store, left, n_lo * sizeof(void *));
                    break;
                }
            }
        }
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("sort() done.", first, nmemb);
#endif
}

// pointer sort
void merge_phybrid(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb > 1) {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT,
                "mi_isort(base=%p, nmemb=%ld, compare) start.\n", base, nmemb);
#endif
        void **idxtbl = calloc(nmemb, sizeof(void *));  // double buffer
        if (idxtbl == NULL) perror(NULL);
        else {
            comp = compare;
            memcpy(idxtbl, base, nmemb * sizeof(void *));   // copy pointers
            sort(base, idxtbl, FALSE, nmemb);
            free(idxtbl);
        }
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "mi_isort() done.\n");
#endif
    }
}

// index sort
void merge_hybrid(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb > 1) {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT,
                "mi_sort(base=%p, nmemb=%ld, size=%ld, compare) start.\n", base, size, nmemb);
#endif
        void **idxtbl = make_index(base, nmemb, size);
        if (idxtbl != NULL) {
            merge_phybrid(idxtbl, nmemb, compare);
            unindex(base, idxtbl, nmemb, size);
            free(idxtbl);
        }
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "mi_sort() done.\n");
#endif
    }
}

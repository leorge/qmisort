/*
 * merge_sort.c
 *
 *  Double buffer merge sort.
 *
 *  Created on: 2013/01/11
 *      Author: leo
 */

#include "sort.h"

static int  (*comp)(const void *, const void *);
static size_t   length;

static void copy(void *dst, const void *src, size_t size)
{
#ifdef  DEBUG
//  qsort_moved += size;
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s, size = %ld)\n",
            dump_data(dst), dump_data(src), size);
#endif
    memcpy(dst, src, size * length); /* restore an elements  */
}

static void sort(void *dst, void *src, bool revert,  size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    if (trace_level >= TRACE_DUMP) dump_array("sort() start in " __FILE__, src, nmemb, length);
#endif
    size_t n_lo = nmemb >> 1;   // = nmemb / 2
    size_t n_hi = nmemb - n_lo;
    sort(dst, src, ! revert, n_lo);
    sort((char *)dst + n_lo * length, (char *)src + n_lo * length, ! revert, n_hi);
    char *store = revert ? src : dst;
#ifdef DEBUG
    char *first = store;        // for debugging
#endif
    char *left = revert ? dst : src;
    char *right = &left[n_lo * length];
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) {
        dump_array("left", left, n_lo, length);
        dump_array("right", right, n_hi, length);
    }
#endif
    while (TRUE) {
        if (comp(left, right) <= 0) {
            copy(store, left, 1); store += length; left += length;      // add one
            if (--n_lo <= 0) {  // empty?
#ifdef DEBUG
                if (trace_level >= TRACE_DUMP) dump_array("append right", right, n_hi, length);
#endif
                copy(store, right, n_hi);   // append remained data
                break;
            }
        }
        else {
            copy(store, right, 1); store += length; right += length;
            if (--n_hi <= 0) {
#ifdef DEBUG
                if (trace_level >= TRACE_DUMP) dump_array("append left", left, n_lo, length);
#endif
                copy(store, left, n_lo);
                break;
            }
        }
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("sort() done.", first, nmemb, length);
#endif
}

void merge_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "merge_sort() nmemb = %ld\n", nmemb);
#endif
    if (nmemb <= 1) return;
    void *dup = calloc(nmemb, size);
    if (dup != NULL) {
        length = size; comp = compare;
        memcpy(dup, base, nmemb * size);
        sort(base, dup, FALSE, nmemb);
        free(dup);
    }
}

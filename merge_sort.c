/*
 * merge_sort.c
 *
 *  Entire merge sort.
 *
 *  Created on: 2013/01/11
 *      Author: leo
 */

#include "sort.h"

static int  (*comp)(const void *, const void *);
static size_t   length;

/* array_sorting */
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

static void asort(void *dst, void *src, bool revert,  size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    if (trace_level >= TRACE_DUMP) dump_array("sort() start in " __FILE__, src, nmemb, length);
#endif
    size_t n_lo = nmemb >> 1;   // = nmemb / 2
    size_t n_hi = nmemb - n_lo;
    asort(dst, src, ! revert, n_lo);
    asort((char *)dst + n_lo * length, (char *)src + n_lo * length, ! revert, n_hi);
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
        asort(base, dup, FALSE, nmemb);
        free(dup);
    }
}

/* index / pointer sorting */
static void psort(void **dst, void **src, bool revert, size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_pointer("sort() start in " __FILE__, src, nmemb);
#endif
    size_t n_lo = nmemb >> 1;   // = nmemb / 2
    size_t n_hi = nmemb - n_lo;
    psort(dst, src, ! revert, n_lo);
    psort(&dst[n_lo], &src[n_lo], ! revert, n_hi);
    void **store = revert ? src : dst;
#ifdef DEBUG
    void **first = store;
#endif
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
            *store++ = *left++;     // add one
            if (--n_lo <= 0) {  // empty?
#ifdef DEBUG
                dump_pointer("append right", right, n_hi);
#endif
                memcpy(store, right, n_hi * sizeof(void *));    // append remained data
                break;
            }
        }
        else {
            *store++ = *right++;
            if (--n_hi <= 0) {
#ifdef DEBUG
                dump_pointer("append left", left, n_lo);
#endif
                memcpy(store, left, n_lo * sizeof(void *));
                break;
            }
        }
    }
#ifdef DEBUG
    dump_pointer("sort() done.", first, nmemb);
#endif
}

/* pointer sorting */
void merge_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    void **idxtbl = calloc(nmemb, sizeof(void *));  // double buffer
    if (idxtbl == NULL) perror(NULL);
    else {
        comp = compare;
        memcpy(idxtbl, base, nmemb * sizeof(void *));   // copy pointers
        psort(base, idxtbl, FALSE, nmemb);
        free(idxtbl);
    }
}

/* index sorting */
void merge_index(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    void **idxtbl = make_index(base, nmemb, size);
    if (idxtbl != NULL) {
        merge_pointer(idxtbl, nmemb, compare);
        unindex(base, idxtbl, nmemb, size);
        free(idxtbl);
    }
}


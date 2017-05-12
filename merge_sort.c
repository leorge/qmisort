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

/* array_sort */
static void copy(void *dst, const void *src, size_t size)
{
#ifdef  DEBUG
//  qsort_moved += size;
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s, size = %s)\n",
            dump_data(dst), dump_data(src), dump_size_t(size));
#endif
    memcpy(dst, src, size * length); /* restore an elements  */
}

static void asort(void *dst, void *src, size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    if (trace_level >= TRACE_DUMP) dump_array("sort() start in " __FILE__, src, nmemb, 0, 0, length);
#endif
    size_t n_lo = nmemb >> 1;   // = nmemb / 2
    size_t n_hi = nmemb - n_lo;
    asort(src, dst, n_lo);
    asort((char *)src + n_lo * length, (char *)dst + n_lo * length, n_hi);
#ifdef DEBUG
    char *first = dst;        // for debugging
#endif
    char *left = src;
    char *right = &left[n_lo * length];
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("merge sub-arrays", left, n_lo, 0, n_hi, length);
#endif
    while (TRUE) {
        if (comp(left, right) <= 0) {
            copy(dst, left, 1); dst += length; left += length;      // add one
            if (--n_lo <= 0) {  // empty?
#ifdef DEBUG
                if (trace_level >= TRACE_DUMP) dump_array("append right", right, n_hi, 0, 0, length);
#endif
                copy(dst, right, n_hi);   // append remained data
                break;
            }
        }
        else {
            copy(dst, right, 1); dst += length; right += length;
            if (--n_hi <= 0) {
#ifdef DEBUG
                if (trace_level >= TRACE_DUMP) dump_array("append left", left, n_lo, 0, 0, length);
#endif
                copy(dst, left, n_lo);
                break;
            }
        }
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_array("sort() done.", first, nmemb, 0, 0, length);
#endif
}

void merge_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "merge_sort() nmemb = %s\n", dump_size_t(nmemb));
#endif
    if (nmemb <= 1) return;
    void *dup = calloc(nmemb, size);
    if (dup != NULL) {
        length = size; comp = compare;
        memcpy(dup, base, nmemb * size);
        asort(base, dup, nmemb);
        free(dup);
    }
}

/* pointer sort */
static void psort(void **dst, void **src, size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_pointer("sort() start in " __FILE__, src, nmemb);
#endif
    size_t n_lo = nmemb >> 1;   // = nmemb / 2
    size_t n_hi = nmemb - n_lo;
    psort(src, dst, n_lo);
    psort(&src[n_lo], &dst[n_lo], n_hi);
#ifdef DEBUG
    void **first = dst;
#endif
    void **left = src;
    void **right = &left[n_lo];
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) {
        dump_pointer("left", left, n_lo);
        dump_pointer("right", right, nmemb - n_lo);
    }
#endif
    while (TRUE) {
        if (n_lo == 0) {
#ifdef DEBUG
            if (trace_level >= TRACE_MOVE) fprintf(OUT, "merge %s\n", dump_data(*right));
#endif
            *dst++ = *right++;
            if (--n_hi == 0) break; // empty
        }
        else if (n_hi == 0) {
#ifdef DEBUG
            if (trace_level >= TRACE_MOVE) fprintf(OUT, "merge %s\n", dump_data(*left));
#endif
            *dst++ = *left++;
            if (--n_lo == 0) break; // empty
        }
        else if (comp(*left, *right) <= 0) {
#ifdef DEBUG
            if (trace_level >= TRACE_MOVE) fprintf(OUT, "merge %s\n", dump_data(*left));
#endif
            *dst++ = *left++;
            n_lo--;
        }
        else {
#ifdef DEBUG
            if (trace_level >= TRACE_MOVE) fprintf(OUT, "merge %s\n", dump_data(*right));
#endif
            *dst++ = *right++;
            n_hi--;
        }
    }
#ifdef DEBUG
    dump_pointer("sort() done.", first, nmemb);
#endif
}

void merge_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    void **idxtbl = calloc(nmemb, sizeof(void *));  // double buffer
    if (idxtbl == NULL) perror(NULL);
    else {
        comp = compare;
        memcpy(idxtbl, base, nmemb * sizeof(void *));   // copy pointers
        psort(base, idxtbl, nmemb);
        free(idxtbl);
    }
}

/* index sort */
void merge_index(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    void **idxtbl = make_index(base, nmemb, size);
    if (idxtbl != NULL) {
        merge_pointer(idxtbl, nmemb, compare);
        unindex(base, idxtbl, nmemb, size);
        free(idxtbl);
    }
}


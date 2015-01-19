/*
 * merge_sort.c - index sort
 *
 *  Created on: 2013/11/03
 *      Author: leo
 */

#include "sort.h"

static int	(*comp)(const void *, const void *);

static void sort(void **dst, void **src, bool revert, size_t nmemb) {
	if (nmemb <= 1) return;
#ifdef DEBUG
	qsort_called++;
	if (trace_level >= TRACE_DUMP) dump_pointer("sort() start in " __FILE__, src, nmemb);
#endif
	size_t n_lo = nmemb >> 1;	// = nmemb / 2
	size_t n_hi = nmemb - n_lo;
	sort(dst, src, ! revert, n_lo);
	sort(&dst[n_lo], &src[n_lo], ! revert, n_hi);
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
		if (comp(*left, *right) < 0) {
			*store++ = *left++;		// add one
			if (--n_lo <= 0) {	// empty?
#ifdef DEBUG
				if (trace_level >= TRACE_DUMP) dump_pointer("append right", right, n_hi);
#endif
				memcpy(store, right, n_hi * sizeof(void *));	// append remained data
				break;
			}
		}
		else {
			*store++ = *right++;
			if (--n_hi <= 0) {
#ifdef DEBUG
				if (trace_level >= TRACE_DUMP) dump_pointer("append left", left, n_lo);
#endif
				memcpy(store, left, n_lo * sizeof(void *));
				break;
			}
		}
	}
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) dump_pointer("sort() done.", first, nmemb);
#endif
}

void merge_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
	if (nmemb <= 1) return;
	void **idxtbl = calloc(nmemb, sizeof(void *));	// double buffer
	if (idxtbl == NULL) perror(NULL);
	else {
		comp = compare;
		memcpy(idxtbl, base, nmemb * sizeof(void *));	// copy pointers
		sort(base, idxtbl, FALSE, nmemb);
		free(idxtbl);
	}
}

void imerge_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
	if (nmemb <= 1) return;
	void **idxtbl = make_index(base, nmemb, size);
	if (idxtbl != NULL) {
		merge_pointer(idxtbl, nmemb, compar);
		unindex(base, idxtbl, nmemb, size);
		free(idxtbl);
	}
}

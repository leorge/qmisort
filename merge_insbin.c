/*
 * mergg_insbin.c
 *
 *	hybrid of merge sort and nibble insertion sort with binray search. Not stable
 *
 *  Created on: 2013/03/24
 *      Author: leo
 */

#include "sort.h"

static int	(*comp)(const void *, const void *);

static void sort(void **dst, void **src, bool revert, size_t nmemb) {
	if (nmemb <= 1) return;
	void **store = revert ? src : dst;	// destination
#ifdef DEBUG
	qsort_called++;
	if (trace_level >= TRACE_DUMP) dump_pointer("sort() start in " __FILE__, src, nmemb);
	void **first = store;
#endif
	if (nmemb <= MAX_SIZE) {	/* Insertion sort */
#ifdef DEBUG
		char	dumpbuf[MAX_SIZE + 1];
#endif
		for (size_t idx = 1; idx < nmemb; idx++) {
			register void *pivot = store[idx];
			// binary-search
			int		ck;
			size_t pos = 0;
			size_t lo = 0, hi = idx - 1;
			while (lo <= hi)
			{	// binary search
				ck = comp(pivot, store[pos = (lo + hi) >> 1]);
				if (ck == 0) break;	// found an equal element.
				else if (ck > 0) lo = pos + 1;
				else if (pos == 0) break;
				else hi = pos - 1;		// ck < 0
			};
			if (ck > 0) pos++;
			// store an index
			void **p = &store[hi = idx], **q = p;
			while (hi-- != pos)
				*p-- = *--q;
			*p = pivot;	// from load[] to store[]
#ifdef DEBUG
			if (trace_level >= TRACE_DUMP) dump_pointer("sorted ", store, idx + 1);
#endif
		}
	}
	else {
		size_t n_lo = nmemb >> 1;	// = nmemb / 2
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
				*store++ = *left++;		// add one
				if (--n_lo <= 0) {	// empty?
#ifdef DEBUG
					if (trace_level >= TRACE_DUMP) dump_pointer("append", right, n_hi);
#endif
					memcpy(store, right, n_hi * sizeof(void *));	// append remained data
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

// pointer sorting
void mi_pbin(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
	if (nmemb > 1) {
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT,
				"mi_pbin(base=%p, nmemb=%ld, compar) start.\n", base, nmemb);
#endif
		void **idxtbl = calloc(nmemb, sizeof(void *));	// double buffer
		if (idxtbl == NULL) perror(NULL);
		else {
			comp = compare;
			memcpy(idxtbl, base, nmemb * sizeof(void *));	// copy pointers
			sort(base, idxtbl, FALSE, nmemb);
			free(idxtbl);
		}
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "mi_pbin() done.\n");
#endif
	}
}

// index sorting
void mi_ibin(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
	if (nmemb > 1) {
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT,
				"mi_ibin(base=%p, nmemb=%ld, size=%ld, compar) start.\n", base, size, nmemb);
#endif
		void **idxtbl = make_index(base, nmemb, size);
		if (idxtbl != NULL) {
			mi_pbin(idxtbl, nmemb, compare);
			unindex(base, idxtbl, nmemb, size);
			free(idxtbl);
		}
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "mi_ibin() done.\n");
#endif
	}
}

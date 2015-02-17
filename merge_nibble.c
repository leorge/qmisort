/*
 * merge_nibble.c
 *
 *	hybrid of merge sort and nibble insertion sort. Stable
 *
 *  Created on: 2015/02/04
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
		void **load = revert ? dst : src;	// source
		INSERT_INDEX index = 0, mask;
		for (size_t idx = 1; idx < nmemb; idx++) {
			register void *pivot = load[idx];
#ifdef DEBUG
			if (trace_level >= TRACE_DUMP) fprintf(OUT, "insert %s at %ld index = %s\n",
				dump_data(pivot), idx, dump_index(dumpbuf, index, idx));
#endif
			// binary-search
			size_t pos = 0;
			for (INSERT_INDEX i = index; pos < idx ; pos++)
				if (comp(pivot, load[i & MASK_DIGIT]) >= 0) break;
				else i >>= DIGIT_WIDTH;
			mask = ~(INSERT_INDEX)0 << (pos *= DIGIT_WIDTH);	// note : LSB of index is largest
#ifdef DEBUG
			if (trace_level >= TRACE_DEBUG) fprintf(OUT, "mask = %llx\tpos = %ld\n", mask, pos / DIGIT_WIDTH);
#endif
			index = ((index & mask) << DIGIT_WIDTH)	// make a gap
				  | (idx << pos) 					// insert subs
				  | (index & ~mask);				// remained part
		}
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "sorted index = %s\n", dump_index(dumpbuf, index, nmemb));
#endif
		for (size_t idx = nmemb; idx > 0; index >>= DIGIT_WIDTH) store[--idx] = load[index & MASK_DIGIT];
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

// pointer sort
void mi_pnibble(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
	if (nmemb > 1) {
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT,
				"mi_isort(base=%p, nmemb=%ld, compar) start.\n", base, nmemb);
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
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "mi_isort() done.\n");
#endif
	}
}

// index sort
void mi_inibble(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
	if (nmemb > 1) {
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT,
				"mi_sort(base=%p, nmemb=%ld, size=%ld, compar) start.\n", base, size, nmemb);
#endif
		void **idxtbl = make_index(base, nmemb, size);
		if (idxtbl != NULL) {
			mi_psort(idxtbl, nmemb, compar);
			unindex(base, idxtbl, nmemb, size);
			free(idxtbl);
		}
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "mi_sort() done.\n");
#endif
	}
}

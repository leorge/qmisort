/*
 * mi_sort.c - index sort
 *
 *	hybrid of merge sort and insertion sort
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
	void **head = store;
#endif
	if (nmemb <= MAX_SIZE) {	/* Insertion sort */
#ifdef DEBUG
		char	dumpbuf[MAX_SIZE + 1];
#endif
		void **load = revert ? dst : src;	// source
		INSERT_INDEX index = 0;
		for (size_t subs = 1; subs < nmemb; subs++) {
			void *pickup = load[subs];
#ifdef DEBUG
			if (trace_level >= TRACE_DUMP) fprintf(OUT, "insert %s index = %s\n",
				dump_data(pickup), dump_index(dumpbuf, index, subs));
#endif
			// binary-search
			int		ck;
			size_t pos = 0, lo = 0, hi = subs - 1;
			while (lo <= hi)
			{	// binary search
				ck = comp(pickup, load[(index >> ((pos = ((lo + hi) >> 1)) * DIGIT_WIDTH)) & MASK_DIGIT]);
				if (ck == 0) break;	// found an equal element.
				else if (ck > 0) lo = pos + 1;
				else if (pos > 0) hi = pos - 1;		// ck < 0
				else break;			// found
			};
			if (ck > 0) pos++;
			INSERT_INDEX mask = ~(INSERT_INDEX)0 << (pos *= DIGIT_WIDTH);
#ifdef DEBUG
			if (trace_level >= TRACE_DEBUG) fprintf(OUT, "mask = %llx\tpos = %ld\n", mask, pos / DIGIT_WIDTH);
#endif
			index = ((index & mask) << DIGIT_WIDTH)	// make a gap
				  | (subs << pos) 					// and insert i
				  | (index & ~mask);				// with remained part
		}
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "sorted index = %s\n", dump_index(dumpbuf, index, nmemb));
#endif
		for (size_t subs = 0; subs <nmemb; index >>= DIGIT_WIDTH) store[subs++] = load[index & MASK_DIGIT];
	}
	else {
		size_t anterior = nmemb >> 1;	// = nmemb / 2
		size_t posterior = nmemb - anterior;
		sort(dst, src, ! revert, anterior);
		sort(&dst[anterior], &src[anterior], ! revert, posterior);
		void **left = revert ? dst : src;
		void **right = &left[anterior];
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) {
			dump_pointer("left", left, anterior);
			dump_pointer("right", right, nmemb - anterior);
		}
#endif
		while (TRUE) {
			if (comp(*left, *right) < 0) {
#ifdef DEBUG
				if (trace_level >= TRACE_DUMP) fprintf(OUT, "add %s\n", dump_data(*left));
#endif
				*store++ = *left++;		// add one
				if (--anterior <= 0) {	// empty?
#ifdef DEBUG
					if (trace_level >= TRACE_DUMP) dump_pointer("append", right, posterior);
#endif
					memcpy(store, right, posterior * sizeof(void *));	// append remained data
					break;
				}
			}
			else {
#ifdef DEBUG
				if (trace_level >= TRACE_DUMP) fprintf(OUT, "add %s\n", dump_data(*right));
#endif
				*store++ = *right++;
				if (--posterior <= 0) {
#ifdef DEBUG
					if (trace_level >= TRACE_DUMP) dump_pointer("append", left, anterior);
#endif
					memcpy(store, left, anterior * sizeof(void *));
					break;
				}
			}
		}
	}
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) dump_pointer("sort() done.", head, nmemb);
#endif
}

// pointer sort
void mi_psort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
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
void mi_isort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
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
